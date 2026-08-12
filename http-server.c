#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>

//defining the size for the client_buf
#define maxBufferLen 1024*75

// Creating address variable of datatype struct sockaddr_in to store port number and ip address
// Server's socket
static struct sockaddr_in server_addr;

// Client's socket
static struct sockaddr_in client_addr;

int main()
{
    // Starting the Network Engine
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        printf("Initialization of Network Engine Failed.Please restart and try again..\n");
        exit(0);
    }
    else
    {
        printf("Network Engine Initialized Successfully\n");
    }

    // Creating Server Socket
    SOCKET server_skt = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (server_skt == INVALID_SOCKET)
    {
        printf("Socket Creation Failed with Error Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
        closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Socket Created Sucessfully. Ready To Bind The Socket\n");

        //setting the socket reuse option variable
        char reuse=1;

        //Modiying server_socket options
        if(setsockopt(server_skt,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse))==SOCKET_ERROR)
        {
            printf("Failed to modify the Socket..May face issues while reconnecting to the same previous client.");
        }
        else
        {
            printf("Socket configurations modified successfully..\n");
        }

    }

    // Cleaning the address variable and setting its complete memory to 0 before assigning the address family, port and ip address
    memset(&server_addr, 0, sizeof(server_addr));

    // Setting up the family adderss,port and ip address values
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8095);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding the Socket to the localhost and a port
    if (bind(server_skt, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("Unable To Bind The Socket\nError Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
        closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Socket Binded Successfully. Ready To Listen to Requests\n");
    }

    // Listening to Socket that is Successfully Binded
    if (listen(server_skt, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Unable To Handle Requests Right Now\n.Error Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
        closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Socket Ready for Listening for New Conection Requests\n");
    }

    // Sizeof client's socket
    int client_sock_len = sizeof(client_addr);
    // Creating Client Socket
    SOCKET client_skt;
    // Accepting and Creating Connection With the Requesting Client
    if ((client_skt = accept(server_skt, (struct sockaddr *)&client_addr, &client_sock_len)) == INVALID_SOCKET)
    {
        printf("Unable to Establish Connection with Accepted Client.Error Code: %d.\nPlease Try Again Later..\n", WSAGetLastError());
        closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Connection Established with new Client. Ready to Receive Data..\n");
    }

    /* Previous Objective
    //Some kind of requests must be received here...
    while(1)
    {
        printf("Waiting for New Connection Requests...\n");
        Sleep(1000);
    }
    */

    // Creating buffer for reaceiving and reading the data.

    /*int maxBufferLen = 1000; - May create problems as it makes the client_buf array to be VLA and VLAs are dangerous as programs are executed in stack and stack has limited size and needs to know how much space is required at run time itself. So use macros at the top.*/

    char client_buf[maxBufferLen]; //Receiving the size from the macro defined at the top of the program.
    int bytesRead;

    /*Current/New Objective
    //Exchanging of data between the server and the client.
    */
    while (1)
    {
        // Cleaning the buffer before reading from it to remove any garbage data present in it.
        memset(client_buf, 0, sizeof(client_buf));

        // Reading from the Buffer
        if (((bytesRead = recv(client_skt, client_buf, sizeof(client_buf) - 1, 0)) == 1) && (client_buf[0] == '`'))
        {
            printf("Disconnecting from the Client..\n");
            closesocket(client_skt);
            WSACleanup();
            printf("Disconnected from the Client Successfully..\n");
            printf("Closing the Server Socket for Safety Purposes..\n");
            closesocket(server_skt);
            exit(0);
        }
        else if (bytesRead == 0)
        {
            printf("Client Disconnected, Terminating the Connection..\n");
            closesocket(client_skt);
            WSACleanup();
            printf("Terminated the Connection Successfully..\n");
            printf("Closing the Server Socket for Safety Purposes..\n");
            closesocket(server_skt);
            exit(0);
        }
        else if (bytesRead == SOCKET_ERROR)
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
            /* Used for checking whether connection is being established from the browser.
            for (int i=0;i<maxBufferLen;i++)
            {
                printf("%c",client_buf[i]);
            }
            */
            // printf("Successfully Read %dbytes from the client..\n",bytesRead);
            // printf("Message: %s\n",client_buf);
        }
    }

    // Closing both the client and the server sockets and Terminating Network Engine
    closesocket(client_skt);
    closesocket(server_skt);
    WSACleanup();
    return 0;
}