#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#define PORT 4444







// function to share the file



void receiveFile(int socket,  char name123[])
{
    printf("+++%s+++", name123);
    char name13[50];
    strcpy(name13, name123);

    write(socket, (void *)&name13, sizeof(name13));

    
    int buffersize = 0, recv_size = 0,size = 0, read_size = 0, write_size;
    char file1array[10241],verify = '1';
    FILE *file1;

    //Find the size of the file1
    read(socket, &size, sizeof(int));



    //Send our verification signal
    write(socket, &verify, sizeof(char));
    //Make sure that the size is bigger than 0
    if(size <= 0 )
    {
        printf("Error has occurred. Size less than or equal to 0\n");
        return;
    }
 


    file1 = fopen(name13, "w");

    if( file1 == NULL) {
        printf("Error has occurred.  file could not be opened\n");
        return;
    }
   


    //Loop while we have not received the entire file yet
    while(recv_size < size) 
    {
        ioctl(socket, FIONREAD, &buffersize); 

        if (buffersize == 10241)
        {
            printf("file successfully Received!\n");
            
           
            break;

        }

        //We check to see if there is data to be read from the socket    
        if(buffersize > 0 ) 
        {
            read_size = read(socket,file1array, buffersize);
            if( read_size < 0)
            {
                printf("%s", strerror(errno));
                return 0;
            }
            
            

            //Write the currently read data into our file1 file
            write_size = fwrite(file1array,1,(buffersize), file1);

            if(write_size != buffersize) 
            {
                printf("write and buffersizes wrong\n");
            }
          
        

            if(read_size != write_size) 
            {
                printf("error in read write\n");
            }
          
        

            //Increment the total number of bytes read
           

                    //Send our handshake verification info
            write(socket, &verify, sizeof(char));

        }
        
        recv_size++;
    
    }

    fclose(file1);
    
    return;
}











int main(int argc , char *argv[])
{


    char logFile[10];
    strcpy(logFile, "log.txt");
    
    char fileName[50];
    int socket_desc;
    struct sockaddr_in server;
    char *parray;
    //decision
    char decision;


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1) 
    {
        printf("Could not create socket");
    }

    memset(&server,0,sizeof(server));
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( PORT );

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) 
    {
        printf("%s",strerror(errno));
        close(socket_desc);
        puts("Connect Error");
        return 1;
    }

    puts("Connected\n");


    printf("to Download press 'd' or 'D':\nto Download the list press 'l' or 'L'\nDECISION>>>");
    scanf(" %c", &decision);
    write(socket_desc, (void *)&decision, sizeof(decision));


    if (decision == 'd' || decision == 'D')
    {
        

        printf("File name: ");
        scanf("%s", &fileName);
        receiveFile(socket_desc, fileName);
        
    }
    else if(decision == 'l' || decision == 'L')
    {



        receiveFile(socket_desc, logFile);

    }
    else
    {
        printf("wrong input\n");
        printf("<<<QUITING>>>>\n");
        close(socket_desc);
        return 0;
    }
    

    
    

    close(socket_desc);

return 0;
}
