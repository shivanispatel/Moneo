#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 3333

struct GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} VmId, ServiceId;


int main(void)
{
    // Get the VM GUID using this command -> (Get-VM -Name $VMName).Id
    // VM GUID: {21f743a1-cf47-4b32-9b69-4cb84412b0d4}
    VmId.Data1 = 569852833; // 0x21f743a1;
    VmId.Data2 = 53063; // 0xcf47;
    VmId.Data3 = 19250; // 0x4b32;
    VmId.Data4[0] = 0x9b;
    VmId.Data4[1] = 0x69;
    VmId.Data4[2] = 0x4c;
    VmId.Data4[3] = 0xb8;
    VmId.Data4[4] = 0x44;
    VmId.Data4[5] = 0x12;
    VmId.Data4[6] = 0xb0;
    VmId.Data4[7] = 0xd4;

    // TO DO : this GUID will need to be updated according to the service
    // Service GUID: {00000d05-efea-4d26-b17d-d547c42e00be}
    ServiceId.Data1 = 533; // 0x00000d05;
    ServiceId.Data2 = 61162; // 0xefea;
    ServiceId.Data3 = 19750; // 0x4d26;
    ServiceId.Data4[0] = 0xb1;
    ServiceId.Data4[1] = 0x7d;
    ServiceId.Data4[2] = 0xd5;
    ServiceId.Data4[3] = 0x47;
    ServiceId.Data4[4] = 0xc4;
    ServiceId.Data4[5] = 0x2e;
    ServiceId.Data4[6] = 0x00;
    ServiceId.Data4[7] = 0xbe;



    int ConnectSocket = -1;
    struct sockaddr_in server;
    const char *sendbuf = "this is a test"; // TO DO : replace this?
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
 
    // add your own input here
    const GUID *vmId = &VmId;
    const GUID *serviceId = &ServiceId;

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
 
 
    // SOCK SPECIFICATIONS ----------------------------------------------------
    // use the Hyper-V socket family and protocol
    ZeroMemory(&clientService, sizeof(clientService));
    clientService.Family = AF_VSOCK;
    clientService.VmId = *vmId;
    clientService.ServiceId = *serviceId;
 
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_VSOCK; // for linux // TO DO : why undefined?
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0; // for linux guest
 
    hints.ai_addrlen = sizeof(SOCKADDR_HV);
    hints.ai_addr = reinterpret_cast<SOCKADDR *>(&clientService);

 
    // CREATE SOCKET ----------------------------------------------------
    ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ConnectSocket == -1) {
        perror("Could not create socket");
        return 1;
    }
 

    // CONNECT TO SERVER ----------------------------------------------------
    if (connect(ConnectSocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
 
    // SEND INITIAL BUFFER ----------------------------------------------------
    // TO DO : this only sends once? does it receive data?
    // TO DO : so sendBuf would need to be modified 
    // create a while loop here to continuously send data?
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        perror("send failed with error.");
        close(ConnectSocket);
        return 1;
    }
 
    printf("Bytes Sent: %ld\n", iResult);
 

    // SHUTDOWN CONNECTION ----------------------------------------------------
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        perror("send failed with error.");
        close(ConnectSocket);
        return 1;
    }
 
    // Receive until the peer closes the connection
    do {
 
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            perror("send failed with error.");
 
    } while (iResult > 0);
 
    // cleanup
    close(ConnectSocket);
 
    return 0;
}

