#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<Windows.h>

//Creating address variable of datatype struct sockaddr_in to store port number and ip address
struct sockaddr_in address;

int main()
{
    //Starting the Network Engine
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
    {
        printf("Initialization of Network Engine Failed.Please Restart and Retry..\n");
        exit(0);
    }
    else
    {
        printf("Network Engine Initialized Successfully\n");
    }


    //Creating Socket
    SOCKET skt=socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    if (skt==INVALID_SOCKET)
    {
        printf("Socket Creation Failed.Please Restart and Retry..\n");
        exit(0);
    }
    else
    {
        printf("Socket Created Sucessfully. Ready To Bind The Socket\n");
    }

    //Cleaning the address variable and setting its complete memory to 0 before assigning the address family, port and ip address
    memset(&address,0,sizeof(address));

    //Setting up the family adderss,port and ip address values
    address.sin_family=AF_INET;
    address.sin_port= htons(8095);
    address.sin_addr.s_addr= INADDR_ANY;

    //Binding the Socket to the localhost and a port
    if(bind(skt,(const struct sockaddr*)&address,sizeof(address))==SOCKET_ERROR)
    {
        printf("Unable To Bind The Socket. Please Restart and Retry..\n");
        closesocket(skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Socket Binded Successfully. Ready To Listen to Requests\n");
    }
    

    //Some kind of requests must be received here...
    while(1)
    {
        printf("Waiting for requests...\n");
        Sleep(1000);
    }
    
    //Closing the socket and Terminating Network Engine
    closesocket(skt);
    WSACleanup();
    return 0;
}