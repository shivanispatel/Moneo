#include <sys/socket.h>
#include <linux/vm_sockets.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 139

/*
struct GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} VmId, ServiceId;
*/


int main(void)
{
    /*
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
    */

    int ConnectSocket = -1;
    struct sockaddr_in server = { 0 };
    struct sockaddr_vm addr = { 0 };

    const char *sendbuf = "this is a test"; // TO DO : replace this?
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
 
    /*
    // add your own input here
    const GUID *vmId = &VmId;
    const GUID *serviceId = &ServiceId;
    */

   // 
   addr.svm_family = AF_VSOCK;
   addr.svm_reserved1 = 0;
   addr.svm_port = DEFAULT_PORT;
   addr.svm_cid = VMADDR_CID_HOST; // 172.25.116.237

   unsigned char svm_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) - sizeof(unsigned short) - sizeof(unsigned int) - sizeof(unsigned int)];
};

 
    // CREATE SOCKET ----------------------------------------------------
    ConnectSocket = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (ConnectSocket == -1) {
        perror("Could not create socket. Error");
        return 1;
    }
    else {
        printf("Socket created\n");
    }
 

    // CONNECT TO SERVER ----------------------------------------------------
    iResult = connect(ConnectSocket, (struct sockaddr*)&addr, sizeof(addr));
    printf("Attempting to connect to server . . .\n");
    if (iResult == -1) {
        perror("Connect failed. Error");
        close(ConnectSocket);
        return 1;
    }
    else {
        printf("Connected to server\n");
    }
 
    // SEND INITIAL BUFFER ----------------------------------------------------
    // TO DO : this only sends once? does it receive data?
    // TO DO : so sendBuf would need to be modified 
    // create a while loop here to continuously send data?
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == -1) {
        perror("send failed. Error");
        close(ConnectSocket);
        return 1;
    }
    else {
        printf("Bytes Sent: %d\n", iResult);
    }


    // SHUTDOWN CONNECTION ----------------------------------------------------
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SHUT_WR);
    if (iResult == -1) {
        perror("shutdown failed with error.");
        close(ConnectSocket);
        return 1;
    }
    else {
        printf("Shutdown successful\n");
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

