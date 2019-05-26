//server implementation

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#define PORT 4444

struct variables
{
    char varname[50];

}st1;

void writeTo()
{
    printf("WRITING...\n");
    FILE *fp1;
    fp1 = fopen("log.txt", "a");
    if(fp1 == NULL)
    {
        return;
    }
    fprintf(fp1, "%s\n", st1.varname);
    fclose(fp1);

}



void receiveFile(int socket)
{
    char namess[50];
    
    printf("waiting for file name\n");
    read(socket, &namess, sizeof(namess));
    printf("\n\nFile name found : %s\n\n", namess);

    int buffersize = 0, recv_size = 0,size = 0, read_size = 0, write_size;
    char imagearray[10241],verify = '1';
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
    else
    {
        printf("file1 size is OK\n");
        printf("size: %d", size);
    }


    file1 = fopen(namess, "w");

    if( file1 == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return;
    }
    else
    {
        printf("file opened\n");
    }


    //Loop while we have not received the entire file yet
    while(recv_size < size) 
    {
        ioctl(socket, FIONREAD, &buffersize); 
        
        if (buffersize == 10241)
        {
            printf("file successfully Received!\n");
            strcpy(st1.varname, namess);
           
            break;

        }

        //We check to see if there is data to be read from the socket    
        if(buffersize > 0 ) 
        {
            read_size = read(socket,imagearray, buffersize);
            if( read_size < 0)
            {
                printf("%s", strerror(errno));
            }
           

            
            

            //Write the currently read data into our file1 file
            write_size = fwrite(imagearray,1,(buffersize), file1);

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
void sendFile(int socket)
{
    char namess[50];
    
    printf("waiting for file name\n");
    read(socket, &namess, sizeof(namess));
    printf("\n\nFile name found : %s\n\n", namess);




    FILE *file2;
    int size=0, read_size=0;
    float avg = 0.00, counter = 0.00;;
    
    char send_buffer[10240], verify;
    char overVariable[10241];
    memset(&overVariable, 0, sizeof(overVariable));

    file2 = fopen(namess, "r");
    printf("Getting file2 Size\n");   

    if(file2 == NULL) {
        printf("Error Opening File");
        return;
    } 

    fseek(file2, 0, SEEK_END);
    size = ftell(file2);


    fseek(file2, 0, SEEK_SET);
    avg = ( float )size / 10;
    if (size <= 0)
    {
        /* code */
        printf("error in size");
        return;
    }
    

    //Send file2 Size
    printf("Sending file2 Size : %d\n", size);
    write(socket, (void *)&size, sizeof(int));

    if(read_size = read(socket, &verify , sizeof(char)) < 0) {
        puts("\nError Receiving Verification");
        return;
    }


    if(verify == '1'){
        printf("sending>>>\n");


        counter = avg;


/**************************************************************************************/


        //Read from the file into our send buffer
            read_size = fread(send_buffer, 1, sizeof(send_buffer), file2);

            
                               

            //Send data through our socket 
            write(socket, send_buffer, read_size);
            
            //Wait for the verify signal to be received 
            while(read(socket, &verify , sizeof(char)) < 0);

                if(verify != '1') {
                    printf("Error Receiving the Handshake signal\n %s",&verify);
                    
                }

            verify = NULL;

            //Zero out our send buffer
            bzero(send_buffer, sizeof(send_buffer));


/******************************************************************************************/

        
        
        while(!feof(file2)) 
        {
            
            //Read from the file into our send buffer
            read_size = fread(send_buffer, 1, sizeof(send_buffer), file2);
  
           
            
                
               

            //Send data through our socket 
            write(socket, send_buffer, read_size);                        

            //Wait for the verify signal to be received 
            while(read(socket, &verify , sizeof(char)) < 0);

                if(verify != '1') 
                {
                    printf("Error Receiving the Handshake signal\n %s",&verify);
                }

            verify = NULL;

            //Zero out our send buffer
            bzero(send_buffer, sizeof(send_buffer));
            
        }
        if(feof(file2))
        {
            write(socket, &overVariable, sizeof(overVariable));
            printf("\n sending done ! \n");
        }
        
    }
    return;   
     
}





int main()
{
    /********************************************************/ 
    //variables
    int socket_desc , new_socket , c, read_size,buffer = 0;
    struct sockaddr_in server , client;
    char *readin;
    char decision;
    pid_t childpid;

    /********************************************************/ 

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc == -1)
    {
        printf("socket not created\n");
        return -1;
    }

    server.sin_family = AF_INET;

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    //binding port
    server.sin_port = htons( PORT );

    //bionding
    int binding  = bind( socket_desc,(struct sockaddr *)&server , sizeof(server) );
    
    //if binding not success
    if(binding < 0)
    {
        printf("Binding failed\n");
        return -1;
    }
    else
    {
        //if binding success

        //connection listening
        listen(socket_desc, 5);
        printf("Listening ...\n");

       while (1)
       {
            c = sizeof(struct sockaddr_in);
            new_socket = accept ( socket_desc , ( struct sockaddr * ) &client , ( socklen_t* ) &c );

             //connected
            if ( new_socket)
            {
                printf("connected:::\n");
             
           

            }
            
        
            fflush(stdout);

             if (new_socket<0)
            {
                 perror("Accept Failed");
                 return 1;
            }

            if (childpid = fork() == 0)
            {
               close(socket_desc);
               while (1)
               {
                    printf("waiting for decision..\n");
                    read(new_socket, &decision, sizeof(int));
                    printf("lecturer made the decision..\n");

                    if(decision == 'u' || decision == 'U')
                    {
                        receiveFile( new_socket );
                        writeTo();
                        printf("log :%s updated", st1.varname);
                        close(new_socket);
                        return 0;
                    }
                    else if (decision == 'd' || decision == 'D')
                    {
                        sendFile(new_socket);
                        close(new_socket);
                        return 0;


                    
                    }
                    else if(decision == 'l' || decision == 'L')
                    {


                        sendFile(new_socket);
                        close(new_socket);
                        return 0;
                    }
                    else
                    {
                         /* code */

                        
                        return 0;
                        
                

                    }
                    decision = NULL;
               }
               

               
            }
            
            
            
            
        }
        close(new_socket);
        fflush(stdout);
       

        

        
        
    }
    

    return 0;
}