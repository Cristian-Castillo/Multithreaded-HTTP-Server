/*
    Cristian C. Castillo & Baubak Saadat
    Professor Nawab
    UCSC CSE 130
    HTTPSERVER Assignment # 1
*/

#ifndef HTTPSERVER_Headers_H
#define HTTPSERVER_Headers_H

/* Socket Setup Error Checking */
bool f_void_vitality_check(char *file_name,char *protocol,char *request,int client_connect);
bool f_bool_check_file(char *parseFile);
bool f_bool_check_file_len(char *fileLength);
bool f_bool_permissions(char *file);
bool f_bool_check_request(char *request);
bool check_http_protocol(char *protocol);
void f_void_server_arg_error();
void f_void_setsocket_error(int setsocket);
void f_void_bind_error(int bind);
void f_void_listen_error(int socket_listen);
void f_void_accept_error(int accept);

/* 200 and 201 Creation/Found*/
void f_client_req_found(int file,int clientsocket);
void f_client_req_created(int int_client_sockd);

/* 400-500 & Error Checks */
void f_void_400(int clientsock);
void f_void_client_error_forbid(int);
void f_void_client_error_not_found(int);
void f_void_intr_error(int);
void f_void_kill_request(int clientsock);
void f_void_path_error(int);
void f_void_getaddrinfo_error(int status,struct addrinfo *res);
void f_void_error_on_accept();

/* New File openings */
int f_fetch_file(char *char_file);

/* Modules & Functionality*/
void *f_void_get_module(char *buffer,char *file_name,int clientSocket);
void *f_void_put_module(char *buffer,char *file_name,int clientSocket);
void *handle_client_request(void * workerId);
void *worker_thread(void *args);
void create_worker(pthread_t *workerArr, int *thread_array, int numWorker);
void f_getopt(int argc, char *argv[], struct command_line_inputs *configs);
bool f_regex_overkill_parse(char * req, char *file,char *protocol);

/* Structs */
struct command_line_inputs;
struct marshall;

struct http_object{
    char* file_name;
    char* request;
    char* protocol;
    char* buffer;
    char* parsed_content;
    int client_connect;
    int key_fd;
    int id;
    int content_length;
    bool flag = false;
    ssize_t write_bytes;
    ssize_t recv_bytes;
    ssize_t send_bytes;
    pthread_mutex_t master_key;
};

#endif