/*
    Cristian C. Castillo & Baubak Saadat
    Professor Nawab
    UCSC CSE 130
    HTTPSERVER Assignment # 1
*/

#ifndef HTTPSERVER_Headers_H
#define HTTPSERVER_Headers_H

/* Socket Setup Error Checking */
bool f_bool_check_file(char *parseFile);
bool f_bool_check_file_len(char *fileLength);
void f_void_server_arg_error();
void f_void_setsocket_error(int setsocket);
void f_void_bind_error(int bind);
void f_void_listen_error(int socket_listen);
void f_void_accept_error(int accept);

/* 200 and 201 Creation/Found*/
void f_client_req_found(int file,int clientsocket);
void f_client_req_created(int int_client_sockd);

/* Regex Check Request */
bool f_regex_overkill_parse(char * req, char *file,char *protocol);

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

/* Modules */
void *f_void_get_module(char *buffer,char *file_name,int clientSocket);
void monolothic_automatic_lock_put(struct http_object *);
void f_void_put_module(struct http_object *http_data);
void handle_all_request(struct marshall *message);
void *handle_server(int fd,struct marshall *message);
void *worker_thread(void *args);

/* Command line Node * Parsing Flags */
void f_getopt(int argc, char *argv[], struct command_line_inputs *configs);


/* Holds redunancy */
struct marshall{
    char *carrier_msg;
    int is_redundant;
    int id;

};

struct command_line_inputs{
    int N_threads;
    bool Redundancy;
};

struct http_object{
    char* file_name;
    char* request;
    char* protocol;
    char* buffer;
    char* parsed_content;
    int client_connect;
    int key_fd;
    int content_length;
    bool flag = false;
    int *array_of_clients;
    ssize_t write_bytes;
    ssize_t recv_bytes;
    pthread_mutex_t master_key;

}http_node;

struct thread_data{
    int id;
    int fd;
    int available;
    pthread_t workers;
    http_object http_node;
}Work_Thread;



#endif
