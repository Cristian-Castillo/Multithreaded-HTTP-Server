/*
    Cristian C. Castillo & Baubak Saadat
    Professor Nawab
    UCSC CSE 130
    HTTPSERVER Assignment # 1
*/

/* C-C++ lib */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex>
#include <getopt.h>
#include <queue>
#include <vector>


/* getaddrinfo - Socket set up e.g ports, etc*/
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

/* Errors & warnings */
#include <errno.h>
#include <err.h>

/* header file included */
#include "httpserver_headers.h"

/* Defined Macros */
#define LIVE 1
#define BACKLOG 500
#define HEADER_BUFFER 4096
#define MAX_BUFFER 32768
#define THREAD_BUFFER 16384

/* POSIX LIB*/
#include <pthread.h>

/* Pthread Global Variables */
pthread_mutex_t mutex;
pthread_cond_t client_in_queue;

/* Global Variables */
std::queue<int>items;
std::queue<char*>module_items;

/* Master http_obj only stores content length */
struct http_node{
    int content_length;
};

/* Holds redunancy */
struct marshall{
    char *master_file;
    char *master_buffer;
    char *master_protocol;
    char *master_request;
    int master_client_connection;
    int master_content_length;
    char *carrier_msg;
    int is_redundant;
};

struct command_line_inputs{
    int N_threads;
    bool Redundancy;
    char *oh_so_redundant;
};

struct http_node spawnNode;

/* Checking valid resource names */
bool f_bool_check_file(char *parseFile){
    for(size_t i = 0; i < strlen(parseFile); i++){
        if((parseFile[i] >= 'A' && parseFile[i] <= 'Z') || (parseFile[i] >= 'a' && parseFile[i] <= 'z') || (parseFile[i] <= '9' && parseFile[i] >= '0')){continue;}
        else{
            perror("Invalid file format name. Try accepted file format range: A-Z, a-z, 0-9.\n");
            return false;
        }
    }
    return true;
}

/* Parse Flags */
void f_getopt(int argc, char *argv[], struct command_line_inputs *configs){

    int option;
    /* Make sure the -N flag followed by a number is utitlized correctly and only once */
    int Nflag = 0; 
    /* Make sure the -r flag is utlizied correctly and only once */
    int Rflag = 0; 

    configs->N_threads = 4;
    configs->Redundancy = false;
    while((option = getopt(argc, argv, "rN:")) != -1){

        switch(option){
            case 'N':
                if(Nflag == 1){
                    fprintf(stderr, "Error: same flag used twice\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    if(atoi(optarg) == 0){
                        fprintf(stderr, "Usage: (-N <integer>)\n");
                        exit(EXIT_FAILURE);
                    }
                    configs->N_threads = atoi(optarg);
                    Nflag += 1;
                }
                continue;
            case 'r':
                if(Rflag == 1){
                    fprintf(stderr, "Error: same flag used twice\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    configs->Redundancy = true;
                    Rflag += 1;
                }
                
                continue;
            default: /* '?' */
                fprintf(stderr, "Options:(-r) or (-N <integer>) \n");
                exit(EXIT_FAILURE);
        }
    }
}

/* Checking file length */
bool f_bool_check_file_len(char *file){
    if(strlen(file) != 10){
        perror("Invalid file format length. Length must be ten characters.\n");
        return false;
    }
    else{
        return true;
    }
}
/* Server argument errors */
void f_void_server_arg_error(){
    perror("Server format failure. \nTry the following:  ./httpserver <ip><port> or sudo ./httpserver <ip> or ./httpserver <ip><port> -N # or ./httpserver\n");  
    exit(EXIT_FAILURE); 
}

/* socket error */
void f_void_socket_error(int int_socket){

    if(int_socket < 0){

        if(errno == ENOTCONN){
        /* Socket is not connected */
            fprintf(stderr, "Socket not connected: %s\n", strerror(errno));
            close(int_socket);
            exit(EXIT_FAILURE);

        }
        else if(errno == EINTR){
        /* interrupted sytem call */
            fprintf(stderr, "Interrupted system call: %s\n", strerror(errno));
            close(int_socket);
            exit(EXIT_FAILURE);
        }
        else if(errno == ENOTSOCK){
        /* Not a socket */
            fprintf(stderr, "Not a socket: %s\n", strerror(errno));
            close(int_socket);
            exit(EXIT_FAILURE);
        }
        else if(errno == EOPNOTSUPP){
        /*Operation not supported on socket */
            fprintf(stderr, "Operation not supported on socket: %s\n", strerror(errno));
            close(int_socket);
            exit(EXIT_FAILURE);
        }
        else{
        perror("Socket operation failed\n");
        close(int_socket);
        exit(EXIT_FAILURE);
        }
    }
    else{
        perror("Socket operation failed\n");
        close(int_socket);
        exit(EXIT_FAILURE);
    }
}

/* set socket error checks*/
void f_void_setsocket_error(int int_setsocket){

    if(int_setsocket < 0){

        if(errno == EBADF){
            fprintf(stderr, "Argument socket is not a valid descriptor.: %s\n", strerror(errno));
            close(int_setsocket);
            exit(EXIT_FAILURE);
        }
        else if(errno == ENOTSOCK){
            fprintf(stderr, "Argument sockfd is a file, not a socket.: %s\n", strerror(errno));
            close(int_setsocket);
            exit(EXIT_FAILURE);
        }
        else{
            perror("Set Socket operation failure\n");
            close(int_setsocket);
            exit(EXIT_FAILURE);
        }
    }
    else{
        perror("Set Socket operation failure\n");
        close(int_setsocket);
        exit(EXIT_FAILURE);
    }
}

/* Bind error checks */
void f_void_bind_error(int bind){

    if(bind < 0){

        if(errno == EADDRINUSE){
            fprintf(stderr, "The given address is already in use.: %s\n", strerror(errno));
            close(bind);
            exit(EXIT_FAILURE);
        }
        else if(errno == EBADF){
            fprintf(stderr, "Socket is not a valid file descriptor: %s\n", strerror(errno));
            close(bind);
            exit(EXIT_FAILURE);
        }
        else if(errno == ENOTSOCK){
            fprintf(stderr, "The file descriptor sockfd does not refer to a socket.: %s\n", strerror(errno));
            close(bind);
            exit(EXIT_FAILURE);
        }
        else if(errno == EINVAL){
            fprintf(stderr, "The socket is already bound to an address.: %s\n", strerror(errno));
            close(bind);
            exit(EXIT_FAILURE);
        }
        else{
            perror("Socket failed to bind\n");
            close(bind);
            exit(EXIT_FAILURE);
        }
    }
    else{
        perror("Socket failed to bind\n");
        close(bind);
        exit(EXIT_FAILURE);
    }
}
/* Listen error */
void f_void_listen_error(int socket_listen){

    if(socket_listen < 0){

        if(errno == EADDRINUSE){
            fprintf(stderr, "Another socket is listening on same port: %s\n", strerror(errno));
            close(socket_listen);
            exit(EXIT_FAILURE);
        }
        else if(errno == EBADF){
            fprintf(stderr, "Socket is not a valid file descriptor: %s\n", strerror(errno));
            close(socket_listen);
            exit(EXIT_FAILURE);
        }
        else if(errno == EOPNOTSUPP){
            fprintf(stderr, "The socket is not of a type that supports the listen() operation.: %s\n", strerror(errno));
            close(socket_listen);
            exit(EXIT_FAILURE);
        }
        else{
            perror("Server failed to start listenting\n");
            close(socket_listen);
            exit(EXIT_FAILURE);
        }
    }
    else{
        perror("Server failed to start listenting\n");
        close(socket_listen);
        exit(EXIT_FAILURE);
    }
}

/* file exist, terminate child process and respond to client 200 */
void f_client_req_found(int fetch_file_fd,int clientSocket){
    
    struct stat buf;
    fstat(fetch_file_fd,&buf);

    int int_ch_content = buf.st_size;
    char server_length[10];

    char msg200[] = "HTTP/1.1 200 OK\r\n";
    char msgContent[] = "Content-Length: ";
    char term[] = "\r\n\r\n";

    sprintf(server_length,"%d",int_ch_content);

    write(clientSocket,msg200,strlen(msg200));
    write(clientSocket,msgContent,strlen(msgContent));
    write(clientSocket,server_length,strlen(server_length));
    write(clientSocket,term,strlen(term));
    return;
}

/* 201 file was created successful ! */
void f_client_req_created(int int_client_sockd){
    dprintf(int_client_sockd,"HTTP/1.1 201 Created\r\nContent-Length: %d\r\n\r\n",0);
    return;
}
/* Overkill parse on request get/put */
bool f_regex_overkill_parse(char * request, char *file,char *protocol){

    char buffer_for_regex[80]; 

        strcpy (buffer_for_regex,request);
        strcat (buffer_for_regex,file);
        strcat (buffer_for_regex,protocol);

        std::regex ea("(GET|PUT)[/]?[a-zA-Z0-9]{10}HTTP/1.1"); 
        bool matcha = regex_match(buffer_for_regex, ea);
        return matcha;
}

/* Client error bad request 400 */
void f_void_400(int client_sockd){
    dprintf(client_sockd, "HTTP/1.1 400 BAD REQUEST\r\nContent-Length: 0\r\n\r\n");
    return;
}

/* Client error forbidden 403 */
void f_void_client_error_forbid(int int_client_sockd){
    dprintf(int_client_sockd, "HTTP/1.1 403 FORBIDDEN\r\nContent-Length: 0\r\n\r\n");
    return;
}

/* Client to server request, file not found */
void f_void_client_error_not_found(int int_client_sockd){
    dprintf(int_client_sockd,"HTTP/1.1 404 FILE NOT FOUND\r\nContent-Length: 0\r\n\r\n");
    return;
}

/* Client error Internal Service Error 500 */
void f_void_intr_error(int int_client_sockd){
    dprintf(int_client_sockd,"HTTP/1.1 500 Internal Service Error\r\nContent-Length: 0\r\n\r\n");
    return;
}

/* Client error bad reqeust */
void f_void_kill_request(int int_client_sockd){
    close(int_client_sockd);
    exit(EXIT_FAILURE);
}

/* Fail to read information from client curl */
void f_void_path_error(int int_client_sockd){
    perror("Read path failure from client file. \n");
    f_void_intr_error(int_client_sockd);
}

/* Check for error on getting the addr info */
void f_void_getaddrinfo_error(int status,struct addrinfo *res){

    if(status < 0){

        if(errno == EAI_AGAIN){
            fprintf(stderr,"The name server returned a temporary failure indication. Try again later: %s\n",strerror(errno));
            freeaddrinfo(res);
            exit(EXIT_FAILURE);
        }
        else if(errno == EAI_SOCKTYPE){
            fprintf(stderr,"The requested socket type is not supported: %s\n",strerror(errno));
            freeaddrinfo(res);
            exit(EXIT_FAILURE);
        }
        else if(errno == EAI_BADFLAGS){
            fprintf(stderr,"hints.ai_flags contains invalid flags: %s\n",strerror(errno));
            freeaddrinfo(res);
            exit(EXIT_FAILURE);
        }
        else if(errno == EAI_SOCKTYPE){
            fprintf(stderr,"Other system error, check errno for details: %s\n",strerror(errno));
            freeaddrinfo(res);
            exit(EXIT_FAILURE);
        }
        else{
            perror("Failed on getaddrinfo.\n");
            freeaddrinfo(res);
            exit(EXIT_FAILURE);
        } 
    }
    else{
        perror("Failed to translate server socket.\n");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    } 
}

void f_void_error_on_accept(){
     char msgAccept[] = "Failed on accept()\n";
     write(1,msgAccept,strlen(msgAccept));
     return;
}

/* open req file as rd only */
int f_fetch_file(char *char_file){
    return open(char_file,O_RDONLY);
}

void f_void_permission_put(char *file_name,int clientSocket){

    struct stat is_file_allowed;
    stat(file_name,&is_file_allowed);

    if(errno != ENOENT && (is_file_allowed.st_mode & S_IWUSR) != S_IWUSR ){
        f_void_client_error_forbid(clientSocket);
    }
}
/* Modularized Put Box */
void *f_void_put_module(char *buffer,char *file_name,int clientSocket){

        /* Check Permissions */
        f_void_permission_put(file_name,clientSocket);

        struct http_node *child;
        child = &spawnNode;

        int content_length;
        char *char_parse_content;

        while((char_parse_content = strtok_r(buffer,"\n", &buffer))){
            sscanf(char_parse_content,"Content-Length: %d",&content_length);
        }
      
        /* Store content-length into struct for long usage */
        child->content_length = content_length;
        
        /* Reset the buffer so that child procees may reuse */
        memset(buffer,0,HEADER_BUFFER); 
        
        int put_fd_request = open(file_name,O_CREAT | O_WRONLY | O_TRUNC,0664);
       
        if(put_fd_request == -1 ){f_void_client_error_not_found(clientSocket);}

        /* ---------------------- Content Length = 0 --------------------------------------------*/

        if(child->content_length == 0){ 
            printf("Check point 1\n");
            f_client_req_created(clientSocket);close(put_fd_request); return NULL;
        }

        /*----------------------- Content Length = n --------------------------------------------*/
            
        if(child->content_length > 0){
            
            ssize_t write_bytes = 0;
            ssize_t check_bytes_recv = 0;
          
            char *buff = (char*)malloc(content_length*sizeof(char*));
            while((check_bytes_recv = read(clientSocket,buff,sizeof(buff))) >= 0){
                write_bytes += write(put_fd_request,buff,check_bytes_recv);
                if(write_bytes == child->content_length)break;
            }
            free(buff);

            if(write_bytes == child->content_length){
		        f_client_req_created(clientSocket);
	            close(put_fd_request);
       		     return NULL;
              
            }
            else{
                f_void_intr_error(clientSocket);
                close(put_fd_request);
                return NULL;
               
            }
        }
        /*------------------------- Content Length = ? Read-> EOF ---------------------------------*/
        else{ 

            ssize_t sendByte = 0;
            ssize_t recv_byte = 0;
            /* Read until end of file  must be manually closed. */
            printf("Check point 3\n");
            while((recv_byte = recv(clientSocket,buffer,sizeof(buffer),0)) != EOF){
                sendByte = write(put_fd_request,buffer,recv_byte);
               // memset(buffer,0,sizeof(buffer));
                if(recv_byte == EOF || sendByte == EOF){
                    close(put_fd_request);
                    return NULL;
                }
            }
        
        }
    return NULL;
}


/* Modularized Get Box */
void *f_void_get_module(char *buffer,char *file_name,int clientSocket){

    struct http_node *child;
    child = &spawnNode;

    memset(buffer,'\0',HEADER_BUFFER);

    /* Open the file for reads only! */
    int fetch_file_fd = f_fetch_file(file_name);

    if(fetch_file_fd == -1){ 
        f_void_client_error_not_found(clientSocket);
        close(fetch_file_fd);
        return NULL; 
    }
    
    /* 200 OK client file found fetch from server */
    if(fetch_file_fd != -1){ 
        
        f_client_req_found(fetch_file_fd,clientSocket);
        ssize_t send_bytes;

        /* Get file size */
        struct stat buf;
        fstat(fetch_file_fd,&buf);
        int get_length = buf.st_size;

        char *buff = (char*)malloc(get_length*sizeof(char*));
        
        /* Store file contents */
        while((child->content_length = read(fetch_file_fd,buff,sizeof(buff))) != 0){
            send_bytes =  write(clientSocket,buff,child->content_length);
            if(send_bytes == get_length){break;}
        }
        close(fetch_file_fd);
            
        if(child->content_length == 0){
            free(buff);
            return NULL;
        }
        else{ 
        /* Internal error of 500 Content failuire */
            f_void_intr_error(clientSocket);
            free(buff);
            return NULL;  
        }
    }
    return NULL;
}

void *worker_thread(void *args){

    /* Unmarshall struct to get message */
    struct marshall *message = (struct marshall *)args;
    char so_redundant;
    int id = *((int *)args); 
	int clientSocket;

	printf("Hullo in %d\n", id);
    if(message->is_redundant == true){ so_redundant = 'r';}
    
    while(LIVE){
           
        /* -----------------  Locking  Thread In ------------------------------*/
        clientSocket = -1;
		printf("Go to loop in %d\n", id);
		pthread_mutex_lock(&mutex);
		{
			while(items.empty()){
				pthread_cond_wait(&client_in_queue,&mutex);
			}
				/* Take 1 client out of the queue */
				clientSocket = items.front();
				items.pop();
		}
		pthread_mutex_unlock(&mutex);
		printf("Worker with id = %d handles socketId = %d\n", id, clientSocket);

       // if(clientSocket >= 0){
       
        /* -----------------  UnLocking Thread In ---------------------------- */

        char buffer[HEADER_BUFFER];

        /* Read client request into buffer */
        if(read(clientSocket,buffer,HEADER_BUFFER) < 0){ f_void_path_error(clientSocket); }
        
        /* Parse HTTP Header request  for GET/PUT - file of requested */
        char request[4],file[50],protocol[10];
        sscanf(buffer,"%s %s %s",request,file, protocol);
        
        /* ------------------------REGEX SECTION ---------------------------- */

        bool matcha = f_regex_overkill_parse(request,file,protocol);
        
        if(matcha != 1){ f_void_400(clientSocket);continue;}

        /*---------------------------------------------------------------------*/

        /* Get the file name - delimiter dash */
     
        char *theFile = file;
        char *restOfString = theFile;
        char *file_name = strtok_r(restOfString,"/", &restOfString);

        /* Check length of file must be = 10 ASCII */
        bool check_file_length = f_bool_check_file_len(file_name);
      
        if(check_file_length == false){
            f_void_400(clientSocket);
            memset(buffer,0,HEADER_BUFFER);
            continue;
        }
      
        /* Checking valid resource names A-Z/a-z/0-9 */
        bool check_file_format = f_bool_check_file(file_name);
        
        if(check_file_format == false){
            f_void_400(clientSocket);
            memset(buffer,0,HEADER_BUFFER);
            continue;
        } 

        if(strcmp(request,"GET") == 0){f_void_get_module(buffer,file_name,clientSocket);continue;}
        if(strcmp(request,"PUT") == 0){ f_void_put_module(buffer,file_name,clientSocket);continue;}
        if((strcmp(request,"PUT") != 0 || ((strcmp(request,"GET") != 0)))) f_void_kill_request(clientSocket); //CC : Future self add res in 2nd parameter if you get malloc its from here
        }
    return NULL;
}
