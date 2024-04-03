#define WIN32_LEAN_AND_MEAN

#define HV_PROTOCOL_RAW 1
#define _WINNT_WIN32 0x0601

#include <winsock2.h>
// #include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
// #include <tchar.h>
// #include <objbase.h>
#include <hvsocket.h>

#ifndef AF_HYPERV
#define AF_HYPERV 34
#endif

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "3333" // TO DO : "27015" from the docs; but when I created the service I used 3333 somewhere?

// VM GUID: {21f743a1-cf47-4b32-9b69-4cb84412b0d4}
DEFINE_GUID(VmId, 0x21f743a1, 0xcf47, 0x4b32, 0x9b, 0x69, 0x4c, 0xb8, 0x44, 0x12, 0xb0, 0xd4);

// TO DO : this GUID will need to be updated according to the service
// Service GUID: {00000d05-efea-4d26-b17d-d547c42e00be}
DEFINE_GUID(ServiceId, 0x00000d05, 0xefea, 0x4d26, 0xb1, 0x7d, 0xd5, 0x47, 0xc4, 0x2e, 0x00, 0xbe);

int main(void)
{     
    WSADATA wsaData;
    
    int iResult;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
 
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
 
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    SOCKADDR_HV sockaddr_hr; // TO DO : note, this used to be called "clientService"
 
    CONST GUID *serviceId = &ServiceId;
 
    // INITIALIZE WINSOCK ----------------------------------------------------
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    else {
        printf("WSAStartup initialized successfully\n");
    }


    // SOCKET SPECIFICATIONS ----------------------------------------------------
    // use the Hyper-V socket family and protocol
    ZeroMemory(&sockaddr_hr, sizeof(sockaddr_hr));
    sockaddr_hr.Family = AF_HYPERV;
    sockaddr_hr.VmId = HV_GUID_CHILDREN; // HV_GUID_ZERO;
    sockaddr_hr.ServiceId = *serviceId;
 
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_HYPERV;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = HV_PROTOCOL_RAW;
    hints.ai_flags = AI_PASSIVE;
 
    hints.ai_addrlen = sizeof(SOCKADDR_HV);
    hints.ai_addr = reinterpret_cast<SOCKADDR *>(&sockaddr_hr);
    
 
    // CREATE SOCKET ----------------------------------------------------

    // Create a SOCKET for the server to listen for client connections
    // ListenSocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    // TO DO : explicitly adding values here for debugging purposes
    ListenSocket = socket(AF_HYPERV, SOCK_STREAM, HV_PROTOCOL_RAW);
    
    // TO DO : the one below is from the documentation, which actually makes use of result
    // ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    // check for errors to see if the socket is a valid socket
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        // freeaddrinfo(result); // TO DO : from documentation
        WSACleanup();
        return 1;
    }
    else {
        printf("socket created successfully\n");
    }
 


    // BIND SOCKET----------------------------------------------------

    // Setup the Hyper-V listening socket
    iResult = bind(ListenSocket, hints.ai_addr, (int)hints.ai_addrlen);

    // Setup the TCP listening socket // TO DO : from documentation -> HV Sockets does not use networking, does not need TCP
    // iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen); // TO DO : from documentation

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        // freeaddrinfo(result); // TO DO : from documentation
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    if (iResult == 0){
        printf("socket bound successfully\n");
    }
    else {
        printf("socket bind failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // TO DO : personal notes : 
    // at this point, the socket is bound to an IP address and port on the system 
    // the server must then listen on that IP address and port for incoming connection requests.



    // LISTEN ON SOCKET----------------------------------------------------
    // TO DO : is this only listening for a single connection? I believe so
    
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("socket listening successfully\n");
    }

    // TO DO : this is the doc version
    /*
    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    */



 
    // ACCEPT CONNECTION ----------------------------------------------------
    // TO DO : add multi threading so that the server can handle multiple clients
    // see the "accepting a connection" official documentation for more details on this
    // When the client connection has been accepted, 
    // a server application would normally pass the accepted client socket 
    // (the ClientSocket variable in the above sample code) 
    // to a worker thread or an I/O completion port and 
    // TO DO : do the above 
    // continue accepting additional connections. 
    // In this basic example, the server continues to the next step.
    // TO DO : is the multi-threading needed for this POC?

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("socket accepted client socket successfully");
    }
 
    // No longer need server socket
    closesocket(ListenSocket);


    // RECEIVE DATA ----------------------------------------------------

    // Receive until the peer shuts down the connection
    do {
 
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
 
            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
 
    } while (iResult > 0);
 

    // SHUTDOWN CONNECTION ----------------------------------------------------
    // TO DO : how long should this connection stay open?

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
 
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
 
    return 0;
}