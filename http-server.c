#include <stdio.h>
#include <stdlib.h>
#include<string.h>
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
        printf("\nSocket Creation Failed with Error Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
        closesocket(server_skt);
        WSACleanup();
        exit(0);
    }
    else
    {
        printf("Socket Created Sucessfully. Ready To Bind The Socket\n");

        //setting the socket reuse option variable
        int reuse=1;

        //Modiying server_socket options
        if(setsockopt(server_skt,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse))==SOCKET_ERROR)
        {
            printf("\nFailed to modify the Socket..May face issues while reconnecting to the same previous client.\nError Code: %d",WSAGetLastError());
        }
        else
        {
            printf("Socket Configurations Modified Successfully\n");
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
        printf("\nUnable To Bind The Socket\nError Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
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
        printf("\nUnable To Handle Requests Right Now\n.Error Code: %d.\nPlease restart and try again..\n", WSAGetLastError());
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

    
    //Creating a forever while loop to keep on accepting new requests
    while(1)
    {
        // Accepting and Creating Connection With the Requesting Client
        if ((client_skt = accept(server_skt, (struct sockaddr *)&client_addr, &client_sock_len)) == INVALID_SOCKET)
        {
            printf("\nUnable to Establish Connection with Accepted Client.Error Code: %d.\nPlease Try Again Later..\n", WSAGetLastError());
            closesocket(server_skt);
            exit(0);
        }
        else
        {
            printf("New Connection Established. Ready to Receive Data..\n");
        }
    
        while (1)
        {
            // Creating buffer for reaceiving and reading the data.
        
            /*int maxBufferLen = 1000; - May create problems as it makes the client_buf array to be VLA and VLAs are dangerous as programs are executed in stack and stack has limited size and needs to know how much space is required at run time itself. So use macros at the top.*/
        
            char *client_buf = malloc(maxBufferLen); //Receiving the size from the macro defined at the top of the program.
            int bytesRead;
        
            // Cleaning the buffer before reading from it to remove any garbage data present in it.
            memset(client_buf, 0, maxBufferLen);
    
            // Reading from the Buffer
            bytesRead = recv(client_skt, client_buf, maxBufferLen - 1, 0);
    
            if (bytesRead == 0)
            {
                printf("\nClient Disconnected..\nClosing Client Socket for Safety Purposes..\n");
                free(client_buf);
                closesocket(client_skt);
                printf("Closed Client Socket Successfully..\n");
                break;
            }
            else if (bytesRead == SOCKET_ERROR)
            {
                printf("\nFacing Network/Socket Issues...\n");
                free(client_buf);
                printf("Closing the Client Socket for Safety Purposes..\n");
                closesocket(client_skt);
                printf("Closed Client Socket Successfully..\n");
                break;
            }
            //Sending Status Bytes
            
            //Checking if we(the server) has received any GET requests and responding back
            else if (strncmp(client_buf,"GET",3)==0)
            {
                //Creating memories for storing the requests
                char filepath[256] = "Web Pages";
                char url[128] = {0};
    
                printf("\nReceived GET Request..\n");
                printf("\n--Client Details--\n");
                // Used for checking the details of connected client
                for (int i=0;i<bytesRead;i++)
                {
                    printf("%c",client_buf[i]);
                }
                printf("--------------------\n");
                printf("\nSending Response to the Client..\n");
                
                //Scanning the request
                sscanf(client_buf,"GET %127s",url);
    
                //Generating the required file name path as per required
                if(strcmp(url,"/")==0)
                {
                    strcat(filepath,"/index.html");
                }
                else
                {
                    strcat(filepath,url);
                    printf("%s",filepath);
                }
                int statusBytesSent;
                int headerBytesSent;
                char stastusLine[]="HTTP/1.1 200 OK\r\n";
                char headers[]="Content-Type: text\r\nContent-Length: ";
                
                //Static (Manually Typed HTML Document).
                // char responseBody[]="<html><body style='display:flex;justify-content:center;align-items:center;'><h1 style='font-size:90px;font-style:oblique;'>Loveday K Ball</h1></body></html>";
                
                //Dynamic (Fetching HTML Document from the files and sending it.)
                
                FILE *file = fopen(filepath,"rb");
                if(file==NULL)
                {
                    //Message if opening the HTML file failed
                    printf("\nError Opening File..\nSending Error Message..");
                    
                    //Creating serverErrorResponseBody and Calculating the size of serverErrorResponseBody
                    char serverErrorResponseBody[]="<html><body style='display:flex;flex-wrap:wrap;justify-content:center;align-items:center;align-content:center;'><h1 style='font-size:90px;font-style:oblique;width:100vw;text-align:center;'>Error: 404</h1><h1 style='font-size:90px;font-style:oblique;width:100vw;text-align:center;'>File Not Found</h1></body></html>";
                    int serverErrorResponseBodyLength=(int)strlen(serverErrorResponseBody);
                    
                    //Converting the calculated value of serverErrorResponseBody into string
                    char serverErrorBodyLength[32];
                    snprintf(serverErrorBodyLength, sizeof(serverErrorBodyLength), "%ld\r\n\r\n", serverErrorResponseBodyLength);
                    
                    //Sending Status Bytes
                    statusBytesSent=send(client_skt,stastusLine,strlen(stastusLine),0);
                    
                    //Sending Header Bytes
                    headerBytesSent=send(client_skt,headers,strlen(headers),0);
                    headerBytesSent+=send(client_skt,serverErrorBodyLength,strlen(serverErrorBodyLength),0);
                    
                    //Checking if status-lines and headers are sent properly
                    if(statusBytesSent==SOCKET_ERROR || headerBytesSent==SOCKET_ERROR)
                    {
                        printf("\nCouldn't Send Response Properly. Reconfigure Server and try again..\nError Code: %d\n",WSAGetLastError());
                    }
                    else
                    {
                        // printf("\nStatus Code OK..\nSending HTTP-Response Body..\n");
                        printf("\nStatus Code OK.. Status-Code Bytes Sent: %d\nHTTP-Header OK.. Header Bytes Sent: %d\n",statusBytesSent,headerBytesSent);
                        send(client_skt,serverErrorResponseBody,serverErrorResponseBodyLength,0);
                        free(client_buf);
                        closesocket(client_skt);
                    }
                    break;                
                }
    
                else
                {
                    long file_size;
                    char *responseBody;
    
                    fseek(file,0,SEEK_END);
                    file_size = ftell(file);
                    rewind(file);
                    responseBody = malloc((file_size)+1);
                    fread(responseBody,1,file_size,file);
                    responseBody[file_size]='\0';
                    
                    //Closing file after reading is completed from file to local buffer
                    fclose(file);
    
                    //Calculating the size of responseBody
                    long responseBodyLength=file_size;
    
                    //Converting the calculated values of responseBody into string
                    char bodyLength[32];
                    snprintf(bodyLength, sizeof(bodyLength), "%ld\r\n\r\n", file_size);
    
                    //Sending Status Bytes
                    statusBytesSent=send(client_skt,stastusLine,strlen(stastusLine),0);
        
                    //Sending Header Bytes
                    headerBytesSent=send(client_skt,headers,strlen(headers),0);
                    headerBytesSent+=send(client_skt,bodyLength,strlen(bodyLength),0);
                    
                    //Checking if status-lines and headers are sent properly
                    if(statusBytesSent==SOCKET_ERROR || headerBytesSent==SOCKET_ERROR)
                    {
                        printf("\nCouldn't Send Response Properly. Reconfigure Server and try again..\nError Code: %d\n",WSAGetLastError());
    
                        //Freeing responseBody meoery to prevent memory leaks
                        free(responseBody);
                    }
                    else
                    {
                        // printf("\nStatus Code OK..\nSending HTTP-Response Body..\n");
                        printf("\nStatus Code OK.. Status-Code Bytes Sent: %d\nHTTP-Header OK.. Header Bytes Sent: %d\n",statusBytesSent,headerBytesSent);
        
                        /*send(client_skt,responseBody,strlen(responseBody),0); - Checking if data is sent*/
        
                        send(client_skt,responseBody,file_size,0);
    
                        //Freeing Dynamically allocated memory for responseBody after sending the response to prevernt memory leaks
                        free(responseBody);
                        free(client_buf);
                        closesocket(client_skt);
                    }
                    break;
                }   
            }
        }    
    }   
    return 0;
}