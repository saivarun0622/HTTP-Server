#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<Windows.h>

//Creating address variable of datatype struct sockaddr_in to store port number and ip address
    //Server's socket
    struct sockaddr_in server_addr;

    //Client's socket
    struct sockaddr_in client_addr;
    
    
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
        
        
        //Creating Server Socket
        SOCKET server_skt=socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
        if (server_skt==INVALID_SOCKET)
        {
            printf("Socket Creation Failed with Error Code: %d.\nPlease Restart and Retry..\n",WSAGetLastError());
            exit(0);
        }
        else
        {
            printf("Socket Created Sucessfully. Ready To Bind The Socket\n");
        }
        
        //Cleaning the address variable and setting its complete memory to 0 before assigning the address family, port and ip address
        memset(&server_addr,0,sizeof(server_addr));
        
        //Setting up the family adderss,port and ip address values
        server_addr.sin_family=AF_INET;
        server_addr.sin_port= htons(8095);
        server_addr.sin_addr.s_addr= INADDR_ANY;
        
        //Binding the Socket to the localhost and a port
        if(bind(server_skt,(const struct sockaddr*)&server_addr,sizeof(server_addr))==SOCKET_ERROR)
        {
            printf("Unable To Bind The Socket\nError Code: %d.\nPlease Restart and Retry..\n",WSAGetLastError());
            closesocket(server_skt);
            WSACleanup();
            exit(0);
        }
        else
        {
            printf("Socket Binded Successfully. Ready To Listen to Requests\n");
        }
        
        //Listening to Socket that is Successfully Binded
        if(listen(server_skt,SOMAXCONN)==SOCKET_ERROR)
        {
            printf("Unable To Handle Requests Right Now\n.Error Code: %d.\nPlease Try Again Later..\n",WSAGetLastError());
            closesocket(server_skt);
            WSACleanup();
            exit(0);
        }
        else
        {
            printf("Socket Ready for Listening for New Conection Requests\n");
        }
        
        //Sizeof client's socket
        int client_sock_len = sizeof(client_addr);
        //Creating Client Socket
        SOCKET client_skt;
        //Accepting and Creating Connection With the Requesting Client
        if((client_skt=accept(server_skt,(struct sockaddr *) &client_addr, &client_sock_len))==INVALID_SOCKET)
        {
            printf("Unable to Establish Connection with Accepted Client.Error Code: %d.\nPlease Try Again Later..\n",WSAGetLastError());
            closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Connection Established with new Client. Ready to Exchange Data..\n");
    }
    
    /* Previous Objective
    //Some kind of requests must be received here...
    while(1)
    {
        printf("Waiting for New Connection Requests...\n");
        Sleep(1000);
    }
    */

   //Creating buffer for reaceiving and reading the data.
   char client_buf[1000];
   int bytesRead;
   
   
   /*Current/New Objective
   //Exchanging of data between the server and the client.
   */
  while(1)
  {
    //Cleaning the buffer before reading from it to remove any garbage data present in it.
    memset(client_buf,0,sizeof(client_buf));

    //Reading from the Buffer
    if(((bytesRead=recv(client_skt,client_buf,sizeof(client_buf)-1,0))==1) && (client_buf[0]=='`'))
    {
        printf("Disconnecting from the Client..\n");
        closesocket(client_skt);
        WSACleanup();
        printf("Disconnected from the Client Successfully..\n");
        printf("Closing the Server Socket for Safety Purposes..\n");
        closesocket(server_skt);
        exit(0);
    }
    else if(bytesRead==0)
    {
        printf("Client Disconnected, Terminating the Connection..\n");
        closesocket(client_skt);
        WSACleanup();
        printf("Terminated the Connection Successfully..\n");
        printf("Closing the Server Socket for Safety Purposes..\n");
        closesocket(server_skt);
        exit(0);
    }
    else if(bytesRead==SOCKET_ERROR)
    {
        printf("Facing Network/Socket Issues...\n");
        closesocket(client_skt);
        WSACleanup();
        printf("Terminated the Connection Successfully..\n");
        printf("Closing the Server Socket for Safety Purposes..\n");
        closesocket(server_skt);
        exit(0);
    }
    else
    {
        printf("Successfully Read %dbytes from the client..\n",bytesRead);
        printf("Message: %s\n",client_buf);
    }
   }
    
    //Closing both the client and the server sockets and Terminating Network Engine
    closesocket(client_skt);
    closesocket(server_skt);
    WSACleanup();
    return 0;
}