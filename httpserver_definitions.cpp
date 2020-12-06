/*
    Cristian C. Castillo 

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* C-C++ lib */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex>
#include <getopt.h>
#include <queue>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <dirent.h>

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

/* Pthread Global Variables & locks*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_put = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_get = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_in_queue = PTHREAD_COND_INITIALIZER;


std::queue<int>items;
std::vector<int>connections;

int Redundency = 0;
int client_connection_count;
int new_thread_count;

/* Master http_obj only stores content length */
struct http_node{
    int content_length;
};

/* Holds redunancy */
struct marshall{
    char *carrier_msg;
    int is_redundant;

};

struct command_line_inputs{
    int N_threads;
    int  Redundancy;
    char *oh_so_redundant;
    int client_recall_connection;
};

struct http_node spawnNode;

/////////////////////////////////////////////////////////////// GET OPT CMD LINE PARSER ///////////////////////////////////////////////////////////

/* Parse Flags */
void f_getopt(int argc, char *argv[], struct command_line_inputs *configs){

    int option;
    /* Make sure the -N flag followed by a number is utitlized correctly and only once */
    int Nflag = 0; 
    /* Make sure the -r flag is utlizied correctly and only once */
    int Rflag = 0; 

    configs->N_threads = 4;
    configs->Redundancy = 0;
    while((option = getopt(argc, argv, "rN:")) != -1){

        switch(option){
            case 'N':
                if(Nflag == 1){
                    fprintf(stderr, "Error: same flag used twice.\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    if(atoi(optarg) == 0){
                        fprintf(stderr, "Usage: (-N <integer>).\n");
                        exit(EXIT_FAILURE);
                    }
                    configs->N_threads = atoi(optarg);
                    Nflag += 1;
                }
                continue;
            case 'r':
                if(Rflag == 1){
                    fprintf(stderr, "Error: same flag used twice.\n");
                    exit(EXIT_FAILURE);
                }
                else{
                    configs->Redundancy = 1;
                    Rflag += 1;
                }
                
                continue;
            default: /* '?' */
                fprintf(stderr, "Options:(-r) or (-N <integer>). \n");
                exit(EXIT_FAILURE);
        }
    }
    return;
}

/////////////////////////////////////////////////////////////// Vitality Modules ///////////////////////////////////////////////////////////

/* Checking Health Status */
bool f_bool_check_vitality(char *file_name,char *protocol,char *request,int client_connect){

        /* Do you have rights to the goods ? */
        bool check_file_access = f_bool_permissions(file_name);
        /* Check length of file must be = 10 ASCII */
        bool check_file_length = f_bool_check_file_len(file_name);
        /* Checking valid resource names A-Z/a-z/0-9 */
        bool check_file_format = f_bool_check_file(file_name);
        /* Check file permissions */
        bool check_request = f_bool_check_request(request);
        /* Check HTTP/1.1 version */
        bool check_protocol = check_http_protocol(protocol);

        if(check_file_access == false){
            f_void_client_error_forbid(client_connect);
            return false;
        }
        else if(check_file_length == false){
            f_void_400(client_connect);
            return false;
        }
        else if(check_file_format == false){
            f_void_400(client_connect);
            return false;
        }
        else if(check_request == false){
            f_void_400(client_connect);
            return false;
        }
        else if(check_protocol == false){
            f_void_400(client_connect);
            return false;
        }
        else {return true;}
}

/* HTTP 1.1 Check protocol */
bool check_http_protocol(char *protocol){
    if((strcmp(protocol,"HTTP/1.1")) != 0){
        return false;
    }else {return true;}
}

/* Request must be Get | Put */
bool f_bool_check_request(char *file_name){
    if((strcmp(file_name,"GET")) != 0 &&  (strcmp(file_name,"PUT")) != 0){return false;}
    else{ return true;}
}

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

/* check permissions of file */
bool f_bool_permissions(char *file_name){

    struct stat is_file_allowed;
    stat(file_name,&is_file_allowed);

    if(errno != ENOENT && (is_file_allowed.st_mode & S_IWUSR) != S_IWUSR ){ return false;}
    else{ return true;}
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

//////////////////////////////////////////////////////// MODULE ERRORS //////////////////////////////////////////////////////////////////


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
     char msgAccept[] = "Warning on accept(). Found bundle for host. Re-using existing connection.\n";
     write(1,msgAccept,strlen(msgAccept));
     return;
}

/////////////////////////////////////////////////// 200 && 201 Modules //////////////////////////////////////////////////

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

//////////////////////////////////////////////////////// UTILITIES /////////////////////////////////////////////////////////////

/* Overkill parse on request Get | Put */
bool f_regex_overkill_parse(char *request, char *file,char *protocol){

    char buffer_for_regex[80]; 

        strcpy (buffer_for_regex,request);
        strcat (buffer_for_regex,file);
        strcat (buffer_for_regex,protocol);

        std::regex ea("(GET|PUT)[/]?[a-zA-Z0-9]{10}HTTP/1.1"); 
        bool matcha = regex_match(buffer_for_regex, ea);
        return matcha;
}


/* open req file as rd only */
int f_fetch_file(char *char_file){
    return open(char_file,O_RDONLY);
}

//////////////////////////////////////////////////////// REDUNDANCY MODULES ///////////////////////////////////////////////////////


/* Redundancy for PUT request */
void f_make_copy(char *file_name, int content_length, int clientSocket){

    /* copy status 0 == file failed to open, status 1 == file opened succesfully */
    int copy1_status, copy2_status, copy3_status;
    copy1_status = copy2_status = copy3_status = 1;  

    ssize_t write_bytes = 0;
    ssize_t check_bytes_recv;
    int get_length = content_length;

    char *buff = (char*)malloc(get_length*sizeof(char*));

/* ----------------------------------------(FILE COPY 1/3) open phase------------------------------------------------- */  
    
    /* checking if copy1 directory exists, if not, we create it */
    struct stat st;
    char dirName[] = "copy";
    char dirCopy1[5];
    strcpy(dirCopy1 ,dirName);
    strcat(dirCopy1, "1");
    if(stat("copy1", &st) == -1){
        mkdir("copy1", 0700);
    }

    /* opening the file in diretory copy1*/
    char path_with_name1[20]; 
    strcpy (path_with_name1,"copy1/");
    strcat (path_with_name1, file_name);
    int file_copy1 = open(path_with_name1, O_CREAT | O_RDWR, 0664);
    
/* -------------------------(FILE COPY 1/3 ) checking length of file to see if == orginal size -----------------------*/

    /* if failed to open, means permission issue, so copy_status goes from 1 to 0 */
    if(file_copy1 == -1){      
        copy1_status = 0;
        close(file_copy1);
    }
    /* if file opened sucessfully */
    else{
        /* write to file in ./copy1 directory */
        write_bytes = 0;
        lseek(file_copy1, 0*sizeof(char), SEEK_SET); //points the pointer of read to the first character (beginning) of the file
        while((check_bytes_recv = read(clientSocket,buff,get_length)) >= 0){
            write_bytes += write(file_copy1,buff,check_bytes_recv);
            if(write_bytes == get_length)break;
        }
        memset(buff, 0, get_length);
    }
/*------------------------------------------ (FILE COPY 1/3 ) DONE ----------------------------------------------------*/
/*-------------------------------------------(COPY FILE 2/3 start) ----------------------------------------------------*/

    char dirCopy2[5];
    strcpy(dirCopy2 ,dirName);
    strcat(dirCopy2, "2");
    if(stat("copy2", &st) == -1){
        mkdir("copy2", 0700);
    }

    char path_with_name2[20]; 

    strcpy (path_with_name2,"copy2/");

    strcat (path_with_name2, file_name);

    int file_copy2 = open(path_with_name2, O_CREAT | O_RDWR, 0664);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    write_bytes = 0;

    if(file_copy2 == -1){
        copy2_status = 0;
        close(file_copy2);
    }
    else{
        /* copy file from ./copy1 directory into ./copy2 directory */
        lseek(file_copy1, 0*sizeof(char), SEEK_SET);
        while((check_bytes_recv = read(file_copy1,buff,get_length)) >= 0){
            write_bytes += write(file_copy2,buff,check_bytes_recv);
            if(write_bytes == get_length)break;
        }
        close(file_copy2);
        memset(buff, 0, get_length);
    }

////////////////////////////////////////////////////////////// (COPY FILE 3/3 START) ///////////////////////////////////////////////////////


    char dirCopy3[5];
    strcpy(dirCopy3 ,dirName);
    strcat(dirCopy3, "3");
    if(stat("copy3", &st) == -1){
        mkdir("copy3", 0700);
    }

    char path_with_name3[20]; 

    strcpy (path_with_name3,"copy3/");

    strcat (path_with_name3, file_name);

    int file_copy3 = open(path_with_name3, O_CREAT | O_RDWR, 0664);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    write_bytes = 0;

    if(file_copy3 == -1){
        copy3_status = 0;
        close(file_copy3);
    }
    else{

        /* copy file from ./copy1 directory into ./copy3 directory */
        lseek(file_copy1, 0*sizeof(char), SEEK_SET);

        while((check_bytes_recv = read(file_copy1,buff,get_length)) >= 0){
            write_bytes += write(file_copy3,buff,check_bytes_recv);
            if(write_bytes == get_length)break;
        }
        close(file_copy3);
        free(buff);
    }
    close(file_copy1);

    /* 3 == 201 OK ,  2 == 201 OK , 1 == 500 error, 0 == 500 error */
    int creation_results = copy1_status + copy2_status + copy3_status; 
    if(creation_results >= 2){  /* if 2 more 3 files were sucesffully written */
        f_client_req_created(clientSocket);
    }
    else if(creation_results == 0 || creation_results == 1){    /* if 0 or 1 files were sucesfully written */
        f_void_intr_error(clientSocket);
    }
}

///////////////////////////////////////////////////// REDUNDANCY GET MODULE ////////////////////////////////////////////////////////////

void *f_check_copy(char *file_name, int clientSocket)
{
    /* 
       if a copy is not found in a directory then copyX_found = 0. 
       If file is found in the directory then copyX_found = 1  for X in (1,2,3)
    */
    int copy1_found, copy2_found, copy3_found;
    copy1_found = 0;
    copy2_found = 0;
    copy3_found = 0;

    /* Checking if the file requested by user exists in ./copy1 directory */
    DIR *dir;
    struct dirent *listD;
    dir = opendir("./copy1");
    
    if(dir == NULL){
        perror("Error: Unable to open current server directory copy1.\n)");
        copy1_found = 0;
    }
    else{
        while((listD = readdir(dir)) != NULL){
            if((strcmp(file_name, listD->d_name) == 0) && (listD->d_type == DT_REG)){
                copy1_found = 1;
            }
        }
    }
    dir = opendir("./copy2");
    
    if(dir == NULL){
        perror("Error: Unable to open current server directory copy2.\n)");
        copy2_found = 0;
    }
    else{
        while((listD = readdir(dir)) != NULL){

            if((strcmp(file_name, listD->d_name) == 0) && (listD->d_type == DT_REG)){
                copy2_found = 1;
            }
        }
    }
    dir = opendir("./copy3");
    
    if(dir == NULL){
        perror("Error: Unable to open current server directory copy3.\n)");
        copy3_found = 0;
    }
    else{
        while((listD = readdir(dir)) != NULL){
            if((strcmp(file_name, listD->d_name) == 0) && (listD->d_type == DT_REG)){
                copy3_found = 1;
            }
        }
    }
    closedir(dir);


    /* 
       If a file's permission is locking us out, then copyX_no_permission = 1. 
       But if we have the permission, copyX_no_permission = 0 
    */
    int copy1_no_permission, copy2_no_permission, copy3_no_permission;
    copy1_no_permission = 0;
    copy2_no_permission = 0;
    copy3_no_permission = 0;

    struct http_node *child;
    child = &spawnNode;

    /* Opening a file in directory ./copy1 */
    char path_with_name1[20]; 
    strcpy (path_with_name1,"copy1/");
    strcat (path_with_name1, file_name);

    int file_copy1 = open(path_with_name1, O_RDONLY,0664);
    if(copy1_found == 1){ /* Indicatese if file was found in directory */
    
    /* if this condition is met that means file in directory but no permission to open  //f_void_client_error_not_found(clientSocket);} */
        if(file_copy1 == -1) {copy1_no_permission = 1;}  
    }
    /* copy1_permission = f_int_permission_put(path_with_name1, clientSocket); */

    /* Opening a file in directory ./copy2 */
    char path_with_name2[20]; 

    strcpy (path_with_name2,"copy2/");

    strcat (path_with_name2, file_name);

    int file_copy2 = open(path_with_name2, O_RDONLY,0664);
    if(copy2_found == 1){
        if(file_copy2 == -1) {copy2_no_permission = 1;} /* {f_void_client_error_not_found(clientSocket);} */
    }
    /* copy2_permission = f_int_permission_put(path_with_name2, clientSocket); */

    /* Opening a file in directory ./copy3 */
    char path_with_name3[20]; 

    strcpy (path_with_name3,"copy3/");

    strcat (path_with_name3, file_name);

    int file_copy3 = open(path_with_name3, O_RDONLY,0664);
    if(copy3_found == 1){
        if(file_copy3 == -1){
            copy3_no_permission = 1; /* {f_void_client_error_not_found(clientSocket);} */
        } 
    }
    /* copy3_permission = f_int_permission_put(path_with_name3, clientSocket); */


    /* getting the content length of each file within each ./copyX directory for X in (1,2,3) */
    struct stat buf1;
    fstat(file_copy1,&buf1);
    int get_length1 = buf1.st_size;
    ////////////////////////////////////////////////////////////////////////////////////
    struct stat buf2;
    fstat(file_copy2,&buf2);
    int get_length2 = buf2.st_size;
    ////////////////////////////////////////////////////////////////////////////////////
    struct stat buf3;
    fstat(file_copy3,&buf3);
    int get_length3 = buf3.st_size;
    ////////////////////////////////////////////////////////////////////////////////////


    char *buff1 = (char*)malloc(get_length1*sizeof(char*));
    char *buff2 = (char*)malloc(get_length2*sizeof(char*));
    char *buff3 = (char*)malloc(get_length3*sizeof(char*));

    /* 
        If two files are found to be the same in regards to content length 
        and ascii character comparison, then copyXY = 1 
    */
    int copy12_same, copy13_same, copy23_same;
    copy12_same = copy13_same = copy23_same = 0;

    ssize_t check_bytes_recv1;
    ssize_t check_bytes_recv2;
    ssize_t check_bytes_recv3;

    /* Checking to see if the the file in ./copy1 is the same in ./copy2 in terms of ascii content and length */
    ssize_t write_bytes = 0;

    if((get_length1 == get_length2) && (file_copy1 != -1) && (file_copy2 !=-1)){
        lseek(file_copy1, 0*sizeof(char), SEEK_SET);
        lseek(file_copy2, 0*sizeof(char), SEEK_SET);
        while(((check_bytes_recv1 = read(file_copy1,buff1,get_length1)) >= 0) && ((check_bytes_recv2 = read(file_copy2,buff2,get_length2)) >= 0) ){
            write_bytes += check_bytes_recv1;
  
            sleep(2);
            /* comparing ascii values */
            if(strcmp(buff1,buff2) != 0){
                break;
            }
            if(write_bytes == get_length1){

                copy12_same = 1;
                break;
            }
        }
    }

     /* 
        Checking to see if the the file in ./copy1 is the same in
        ./copy3 in terms of ascii content and length 
    */
    memset(buff1, 0, get_length1);
    memset(buff2, 0, get_length2);
    
    if(copy12_same == 0 && (get_length1 == get_length3)  && (file_copy1 != -1) && (file_copy3 !=-1)){
        
            lseek(file_copy1, 0*sizeof(char), SEEK_SET);
            lseek(file_copy3, 0*sizeof(char), SEEK_SET);
            write_bytes = 0;
            
            while((check_bytes_recv1 = read(file_copy1,buff1,get_length1)) >= 0 && (check_bytes_recv3 = read(file_copy3,buff3,get_length3)) >= 0){
            write_bytes += check_bytes_recv1;
            
            if(strcmp(buff1,buff3) != 1){
                break;
            }
            if(write_bytes == get_length1){
    
                copy13_same = 1;
                break;
            }
        }
    }

    /* Checking to see if the the file in ./copy2 is the same in ./copy3 in terms of ascii content and length */
    memset(buff1, 0, get_length1);
    memset(buff3, 0, get_length3);
    
    if((copy12_same == 0) && (copy13_same == 0) && (get_length2 == get_length3)  && (file_copy2 != -1) && (file_copy3 !=-1)){
        
        lseek(file_copy2, 0*sizeof(char), SEEK_SET);
        lseek(file_copy3, 0*sizeof(char), SEEK_SET);
        write_bytes = 0;
        

        while((check_bytes_recv2 = read(file_copy2,buff2,get_length2)) >= 0 && (check_bytes_recv3 = read(file_copy3,buff3,get_length3)) >= 0){
            write_bytes += check_bytes_recv2;

            sleep(2);
            if(strcmp(buff2,buff3) != 0){
                break;
            }
            if(write_bytes == get_length2){
  
                copy23_same = 1;
                break;
            }
        }
    }
    memset(buff1, 0, get_length1);
    memset(buff3, 0, get_length3);

    lseek(file_copy1, 0*sizeof(char), SEEK_SET);
    lseek(file_copy2, 0*sizeof(char), SEEK_SET);
    
    free(buff1);
    free(buff2);
    free(buff3);

    close(file_copy3);

    /* 
       NOTE: the number of files that gives us no permission 
       to open is always less than or equal to the number of files that exist 
    */ 

    /* calculates the number of files that gives no permission for us to open in directorys ./copyX */
    int num_of_no_permission_files = copy1_no_permission + copy2_no_permission + copy3_no_permission;

    /* calculates the number of files that exists in the directory ./copyX */
    int num_of_files_that_exist = copy1_found + copy2_found + copy3_found;

    /* If all three files are not found or no permission, the status code should be 500 */
    if((num_of_files_that_exist == 0) || (num_of_no_permission_files == 3)){
       /* printf("500 internal error\n"); */
        close(file_copy1);
        close(file_copy2);
        f_void_intr_error(clientSocket);
    }

    /*  GET file with 2/3 or 3/3 files giving no permissions  */
    if(num_of_no_permission_files >=2 ){                
        /* printf("403 error per post @385 endosred by Daniel Alvs #2 & 3 ALSO @336 #1\n");*/
        close(file_copy1);
        close(file_copy2);
        f_void_client_error_forbid(clientSocket);
    }

    /*  GET file with 1/3 files giving no permissions, 1/3 files that don't exist, 1/3 files that do exist and we have permission */
    if((num_of_files_that_exist == 2) && (num_of_no_permission_files == 1)){                
        /* printf("500 error per post @385 endosred by Daniel Alvs #4 ALSO @336 #2\n"); */
        close(file_copy1);
        close(file_copy2);
        f_void_intr_error(clientSocket);
    }

    /*  GET with 2/3 files that don't exist, and file with 1/3 files giving no permissions   */
    if((num_of_files_that_exist == 1) && (num_of_no_permission_files == 1)){                
        /* printf("404 error per post @385 endosred by Daniel Alvs #5 ALSO @336 #3\n"); */
        close(file_copy1);
        close(file_copy2);
        f_void_client_error_not_found(clientSocket);
    }

    if(copy12_same == 1 || copy13_same == 1){

        /* sending HTTP 200 */
        f_client_req_found(file_copy1,clientSocket); 
        ssize_t send_bytes = 0;

        char *buffs = (char*)malloc(get_length1*sizeof(char*));

        /* Store file contents */
        while((child->content_length = read(file_copy1,buffs,sizeof(buffs))) != 0){

            /* printf("if you see then that means i am writing into the client socket\n"); */
            send_bytes +=  write(clientSocket,buffs,child->content_length);
            if(send_bytes == get_length1){break;}
        }
        close(file_copy1);
        close(file_copy2);
            
        if(child->content_length == 0){
            free(buffs);
            return NULL;
        }
        else{ 
            /* Internal error of 500 Content failuire*/
            f_void_intr_error(clientSocket);
            free(buffs);
            return NULL;   
        }

    }
    else if(copy23_same == 1){

        f_client_req_found(file_copy2,clientSocket);
        ssize_t send_bytes;

        char *buffs = (char*)malloc(get_length2*sizeof(char*));
        
        /* Store file contents */
        while((child->content_length = read(file_copy2,buffs,sizeof(buffs))) != 0){
            send_bytes =  write(clientSocket,buffs,child->content_length);
            if(send_bytes == get_length2){break;}
        }
        close(file_copy1);
        close(file_copy2);
            
        if(child->content_length == 0){
            free(buffs);
            return NULL;
        }
        else{ 
            /* Internal error of 500 Content failuire */
            f_void_intr_error(clientSocket);
            free(buffs);
            return NULL;
                
        }
    }
    else{
        close(file_copy1);
        close(file_copy2);
        f_void_intr_error(clientSocket);
        return NULL;
    }
}

/////////////////////////////////////////////////////////////// PUT MODULE //////////////////////////////////////////////////////////////////

void *f_void_put_module(char *buffer,char *file_name,int clientSocket){

            struct http_node *child, content_len_mem;
            struct http_object *http_ptr,http_mem_location;
            http_ptr = &http_mem_location;
            child = &content_len_mem;
            http_ptr->file_name = file_name;
            http_ptr->client_connect = clientSocket;
            http_ptr->buffer = buffer;
            http_ptr->master_key = lock_put;

            int content_length;
            char *char_parse_content = strtok(buffer,"\n");
          
            while(char_parse_content != NULL){
                 sscanf(char_parse_content,"Content-Length: %d",&content_length);
                 char_parse_content = strtok (NULL, "\n");
            }
            free(&char_parse_content);

            /* Redundency */
            if(Redundency == 1){
                f_make_copy(file_name, content_length, clientSocket);
                return NULL;
            }

            http_ptr->content_length = content_length;

            /* Reset the buffer so that child procees may reuse */
            memset(http_ptr->buffer,'\0',HEADER_BUFFER); 
            
            http_ptr->key_fd = open(http_ptr->file_name,O_CREAT | O_WRONLY | O_TRUNC,0664);

            if(http_ptr->key_fd == -1 ){
                pthread_mutex_lock(&http_ptr->master_key);
                {
                    f_void_client_error_not_found(http_ptr->client_connect);return NULL;
                    close(http_ptr->key_fd);
                }
                pthread_mutex_unlock(&http_ptr->master_key);
                return NULL;
            }

            /* ---------------------------- Content Length = 0 ------------------------------------------- */

            if(http_ptr->content_length == 0){ 

                pthread_mutex_lock(&http_ptr->master_key);
                {
                    f_client_req_created(http_ptr->client_connect);
                    close(http_ptr->key_fd); return NULL;
                }
                pthread_mutex_unlock(&http_ptr->master_key);
            }
 
            if(http_ptr->content_length > 0){
         
            /* ---------------------------- Content Length = n -------------------------------------------- */
           
                char *buff = (char*)malloc(http_ptr->content_length*sizeof(char*));
             
                pthread_mutex_lock(&http_ptr->master_key);
                {
                        http_ptr->write_bytes = 0;
                        while((http_ptr->recv_bytes = read(http_ptr->client_connect,buff,http_ptr->content_length)) >= 0){
                            http_ptr->write_bytes += write(http_ptr->key_fd,buff,http_ptr->recv_bytes);
                            if(http_ptr->write_bytes == http_ptr->content_length)break;
                        }
                }
                pthread_mutex_unlock(&http_ptr->master_key);

                if(http_ptr->write_bytes == http_ptr->content_length){
                    
                    pthread_mutex_lock(&http_ptr->master_key);
                    {
                        f_client_req_created(http_ptr->client_connect);
                        free(buff);
                        close(http_ptr->key_fd);
                    }
                    pthread_mutex_unlock(&http_ptr->master_key);
                    return NULL;
                }
                else{

                    pthread_mutex_lock(&http_ptr->master_key);
                    {
                        f_void_intr_error(clientSocket);
                        free(buff);
                        close(http_ptr->key_fd);
                    }
                    pthread_mutex_unlock(&http_ptr->master_key);
                    return NULL;
                }
            }
            /*------------------------- Content Length = ? -> Assume 16 Kib allottted Buffer ---------------------------------*/
            else{ 

                char *buff = (char*)malloc(THREAD_BUFFER*sizeof(char*));
                
                 pthread_mutex_lock(&http_ptr->master_key);
                {
                        http_ptr->write_bytes = 0;

                        while((http_ptr->recv_bytes = read(http_ptr->client_connect,buff,THREAD_BUFFER)) >= 0){
                            http_ptr->write_bytes += write(http_ptr->key_fd,buff,http_ptr->recv_bytes);
                            if(http_ptr->write_bytes == http_ptr->recv_bytes)break;
                        }
                }
                pthread_mutex_unlock(&http_ptr->master_key);

                if(http_ptr->write_bytes == http_ptr->recv_bytes){
                    
                    pthread_mutex_lock(&http_ptr->master_key);
                    {
                        f_client_req_created(http_ptr->client_connect);
                        free(buff);
                        close(http_ptr->key_fd);
                    }
                    pthread_mutex_unlock(&http_ptr->master_key);
                    return NULL;
                }
                else{

                    pthread_mutex_lock(&http_ptr->master_key);
                    {
                        f_void_intr_error(clientSocket);
                        free(buff);
                        close(http_ptr->key_fd);
                    }
                    pthread_mutex_unlock(&http_ptr->master_key);
                    return NULL;
                }
            }
            return NULL;
}

/////////////////////////////////////////////////////////////// GET MODULE //////////////////////////////////////////////////////////////////

void *f_void_get_module(char *buffer,char *file_name,int clientSocket){

    struct http_object *http_ptr,http_mem_location;
    http_ptr = &http_mem_location;

    struct http_node *child, content_len_mem;
    child = &content_len_mem;
    http_ptr->master_key = lock_get;

    memset(buffer,'\0',HEADER_BUFFER);

    if(Redundency == 1){
        f_check_copy(file_name, clientSocket);
        return NULL;
    }

    /* Open the file for reads only! */
    int fetch_file_fd = f_fetch_file(file_name);

    if(fetch_file_fd == -1){ 
        pthread_mutex_lock(&http_ptr->master_key);
        {
            f_void_client_error_not_found(clientSocket);
            close(fetch_file_fd);
            pthread_mutex_unlock(&http_ptr->master_key);
        }
        return NULL; 
    }
    
    /* 200 OK client file found fetch from server */
    if(fetch_file_fd != -1){ 

        int get_length;
         /* Get file size  & 200 OK */
        pthread_mutex_lock(&http_ptr->master_key);
        {
            f_client_req_found(fetch_file_fd,clientSocket);
            struct stat buf;
            fstat(fetch_file_fd,&buf);
            get_length = buf.st_size;
        }
        pthread_mutex_unlock(&http_ptr->master_key);

     
        pthread_mutex_lock(&http_ptr->master_key);
        char *buff = (char*)malloc(get_length*sizeof(char*));
        ssize_t send_bytes;

        /* Store file contents */
        while((child->content_length = read(fetch_file_fd,buff,sizeof(buff))) != 0){
            send_bytes =  write(clientSocket,buff,child->content_length);
            if(send_bytes == get_length){break;}
        }
        pthread_mutex_unlock(&http_ptr->master_key);

        if(child->content_length == 0){
            pthread_mutex_lock(&http_ptr->master_key);
            {
                free(buff);
                close(fetch_file_fd);
                
            }
            pthread_mutex_unlock(&http_ptr->master_key);
            return NULL;
        }
        else{ 
        /* Internal error of 500 Content failuire */
        pthread_mutex_lock(&http_ptr->master_key);
        {
            free(buff);
            f_void_intr_error(clientSocket);
         
        }
        pthread_mutex_unlock(&http_ptr->master_key);
        return NULL; 
        }
    }
    return NULL;
}

/////////////////////////////////////////////// Create Worker | Dispatcher Thread Pool //////////////////////////////////////////////////////

void create_worker(pthread_t *workerArr, int *thread_array,struct command_line_inputs *configs){
                                       
    size_t s1;                                                           
    pthread_attr_t attr;                                                            
    char msg[100];
    int index,ret,rc;       
    int numWorker = configs->N_threads;
    free(&configs);                                               
                                                                                
   rc = pthread_attr_init(&attr);                                               
   
   if(rc == -1){                                                              
      perror("error in pthread_attr_init");                                     
      exit(1);                                                                  
   }                                                                            
    /* Make thread safe set stack size attributes */                                                               
   s1 = 4096;                                                                   
   rc = pthread_attr_setstacksize(&attr, s1);                                   
   
   if(rc == -1){                                                              
      perror("error in pthread_attr_setstacksize");                             
      exit(2);                                                                  
   } 
    /* Dispatcher - N Thread Pool */
	for(index = 0; index < numWorker; index++){
        
        thread_array[index] = index;
        ret = pthread_create(&workerArr[index],&attr, worker_thread, &thread_array[index]);
        
        if(ret){
			sprintf(msg, "Failed on pthread_create()%d\n", ret);
            exit(EXIT_FAILURE); 
		}		
	}
    free(&thread_array); 
    free(&workerArr);
}


/////////////////////////////////////////////// Worker Thread | Process Thread Request //////////////////////////////////////////////////////

void *worker_thread(void *args){
    
    struct http_object *node_parser,get_http_content;
    node_parser = &get_http_content;

    int id = *((int *)args); 
	int clientSocket;
    


    while(LIVE){
           
        /* -----------------  Locking  Thread In ------------------------------*/
        clientSocket = -1;
		
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

       
        /* -----------------  UnLocking Thread In ---------------------------- */
    
        char buffer[HEADER_BUFFER];
        size_t bytes_read;
        int msgsize = 0;

        /* read the clients message */
        while((bytes_read = read(clientSocket,buffer+msgsize,sizeof(buffer)-msgsize-1)) > 0){
            msgsize += bytes_read;
            if(msgsize > HEADER_BUFFER-1 || buffer[msgsize-1] == '\n')break;
        }
        buffer[msgsize-1] = 0; /* null terminate message and remove the \n */

        char request[4],file[50],protocol[10];
        sscanf(buffer,"%s %s %s",request,file, protocol);
     
        node_parser->buffer = buffer;
        node_parser->protocol = protocol;
        node_parser->request = request;
        node_parser->file_name = file;
        node_parser->client_connect = clientSocket;
        node_parser->id = id;

        /* ------------------------ REGEX SECTION ---------------------------- */

        bool matcha = f_regex_overkill_parse(node_parser->request,node_parser->file_name,node_parser->protocol);
        
        if(matcha != 1){ f_void_400(node_parser->client_connect); return NULL;}

        /*---------------------------------------------------------------------*/

        /* Get the file name - delimiter dash */
     
        char *theFile = file;
        char *restOfString = theFile;
        char *file_name = strtok_r(restOfString,"/", &restOfString);
        node_parser->file_name = file_name;
        node_parser->buffer = buffer;
        node_parser->client_connect = clientSocket;
        

        /* Master Module Check Box - Thread Dies on failure on failure */
        bool die_on_failure = f_bool_check_vitality(node_parser->file_name,node_parser->protocol,node_parser->request,node_parser->client_connect);
        
        if(die_on_failure == false){
            memset(node_parser,0,sizeof(http_object));
            continue;
        }
 
        /* Tokenizes my string please */
        std::string string_to_parse;
        std::stringstream ss;
        ss << node_parser->file_name;
        ss >> string_to_parse;
        
        /* Fancy hashing */
        std::unordered_map<std::string,int>hashing;

        /* hash id [file n] = id & convert too c-str */
        hashing[string_to_parse.c_str()] = node_parser->id;
        
        /* initialize iterator to search up valid existing id keys */
        std::unordered_map<std::string, int>::iterator lookup;

        for(lookup = hashing.begin(); lookup != hashing.end(); lookup++){ 
            

            if(strcmp(node_parser->request,"PUT") == 0 && strcmp(node_parser->request,"GET") != 0){
                f_void_put_module(buffer,node_parser->file_name,node_parser->client_connect);
                memset(node_parser,0,sizeof(http_object));
                return NULL;
            }

            if(strcmp(node_parser->request,"GET") == 0 && strcmp(node_parser->request,"PUT") != 0){
                f_void_get_module(buffer,node_parser->file_name,node_parser->client_connect);
                memset(node_parser,0,sizeof(http_object));
                return NULL;
            }   
        }
    }
    return NULL;
}
