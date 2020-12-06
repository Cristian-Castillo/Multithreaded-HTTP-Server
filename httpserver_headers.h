/*
    Cristian C. Castillo 
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HTTPSERVER_Headers_H
#define HTTPSERVER_Headers_H

////////////////////////////////////////////////// ERROR HEADERS ///////////////////////////////////////////////////////////////
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
void f_void_error_on_accept();
void f_void_path_error(int);
void f_void_getaddrinfo_error(int status,struct addrinfo *res);

////////////////////////////////////////////////// 200 & 201 Components ////////////////////////////////////////////////////////
void f_client_req_found(int file,int clientsocket);
void f_client_req_created(int int_client_sockd);

////////////////////////////////////////////////// HTTP Client Response Error Components ///////////////////////////////////////

void f_void_400(int clientsock);
void f_void_client_error_forbid(int);
void f_void_client_error_not_found(int);
void f_void_intr_error(int);
void f_void_kill_request(int clientsock);


////////////////////////////////////////////////// Module Operation Components /////////////////////////////////////////////////

void *f_void_get_module(char *buffer,char *file_name,int clientSocket);
void *f_void_put_module(char *buffer,char *file_name,int clientSocket);
void *handle_client_request(void * workerId);
void *worker_thread(void *args);
void create_worker(pthread_t *workerArr, int *thread_array,struct command_line_inputs *configs);
void f_getopt(int argc, char *argv[], struct command_line_inputs *configs);
bool f_regex_overkill_parse(char * req, char *file,char *protocol);
int f_fetch_file(char *char_file);

////////////////////////////////////////////////// Universal Storage Structs ////////////////////////////////////////////////////
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