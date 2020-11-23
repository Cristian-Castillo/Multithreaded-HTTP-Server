/*
    Cristian C. Castillo & Baubak Saadat
    Professor Nawab
    UCSC CSE 130
    HTTPSERVER Assignment 2
*/

#include "httpserver_definitions.cpp"

#define HEADER_BUFFER 4096
#define MAX_POOL 50

pthread_t thread_pool[MAX_POOL];
static int master_counter = 0;

struct node{
    struct node *next;
    int *clientSocket;

};
typedef struct node node_t;

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(int *client_socket){
    node_t *newnode = (node_t*)malloc(sizeof(node_t));
    newnode->clientSocket = client_socket;
    newnode->next = NULL;
    if(tail == NULL){
        head = newnode;
    }
    else{
        tail->next = newnode;
    }
    tail = newnode;
}

int* dequeue(){
    if(head == NULL){
        return NULL;
    }
    else{
        int *result = head->clientSocket;
        node_t *temp = head;
        head = head->next;
        if(head ==NULL){
            tail = NULL;
        }
        free(temp);
        return result;
    }
}

/* Function prototypes */
void *handle_connection_task(void* clientSocket);
void *worker_threads(void *arg);

int main(int argc, char **argv){

    struct http_object *node_parser,get_http_content;
    node_parser = &get_http_content;
    /* Command Line Struct Data */
    struct command_line_inputs *cmd,get_cmd;
    cmd = &get_cmd;

    /* Port Number and Configurations */
    const char *portNumber;
    const char *host_address;

    int counter_left_off = 0;
    for(int j = 1; j< argc; j++){  
        if (strlen(argv[j]) >= 7 && strlen(argv[j]) <= 15){
            
            std::regex ea("(localhost|[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3})"); 
            bool match = regex_match(argv[j], ea);
            if (match == 1){
                host_address = argv[j];
                counter_left_off = j;
                break;
            }
            else{
                perror("Invalid <ip address> format\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    if(counter_left_off == 0){
        printf("INVALID IP ADDRESS OR NOT ENTERED.\n");
        exit(EXIT_FAILURE);
    }
    portNumber = "80";
    for(int j = counter_left_off; j < argc; j++){    
        if (strlen(argv[j]) == 4){
            portNumber = argv[j];
            break;
        }
    }

    /*Get structs rdy */
    struct sockaddr_storage client_addr;
    struct addrinfo hints, *res;  
    socklen_t addr_size;
    int enable = 1; 
    int serverSocket,ret;
    
    /* Configuration host and port number */
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    if((ret = getaddrinfo(host_address,portNumber,&hints,&res)) != 0){ f_void_getaddrinfo_error(ret,res);}

    /* Flag Parse CMD Line Args */
    f_getopt(argc, argv, &get_cmd);
    printf("Does this work : %d\n",cmd->N_threads);

    /* Create socket */
    serverSocket = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(serverSocket < 0){ f_void_socket_error(serverSocket);}   

    /* Reuse socket addr*/
    if((ret=setsockopt(serverSocket, SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int))) < 0){
        f_void_setsocket_error(ret);
    }
    
    /* Bind socket */
    if((ret = bind(serverSocket,res->ai_addr,res->ai_addrlen)) < 0 ){ f_void_bind_error(ret); }

    /* Listen to sock & backlog */
    if((ret = listen(serverSocket,BACKLOG)) == -1){ f_void_listen_error(ret);}

    /* Marshall the struct - get rdy to pack msg */
    struct marshall *message = (struct marshall *)malloc(sizeof(marshall)); 
    
    if(cmd->Redundancy == true){message->is_redundant = cmd->Redundancy;}

    // create a bunc hfo threads to handle future connections
    for(int i = 0; i < MAX_POOL;i++){
        pthread_create(&thread_pool[i],NULL,worker_threads,NULL);
    }

    int clientSocket = 0;
  
    while(LIVE){
        
            printf("Waiting for connections ...\n");
            if((clientSocket = accept(serverSocket,(struct sockaddr *)&client_addr, &addr_size)) < 0){
                perror("Error on accept()\n");
            }
            // while(clientSocket < 0){
            //     pthread_cond_wait(&condition_v,&mutex_req);
            // }
            printf("Connected to client socket: %d\n",clientSocket);;
            int *pclient = ((int*)malloc(sizeof(int)));
            *pclient = clientSocket;
            pthread_mutex_lock(&mutex_req);
            items.push(*pclient);
            enqueue(pclient);
            pthread_cond_signal(&condition_v); // jump thread back in
            pthread_mutex_unlock(&mutex_req);
  
    }
    return 0;
}


void *worker_threads(void *arg){

    struct marshall *un_pack_message = (struct marshall*)arg;

    while(LIVE){
        int *pclient;
        pthread_mutex_lock(&mutex_req);
        if((pclient = dequeue()) == NULL){
            pthread_cond_wait(&condition_v,&mutex_req);
            pclient = dequeue();
        }
        pthread_mutex_unlock(&mutex_req);
        if(pclient != NULL){
            // we have a connection
            handle_connection_task(pclient);
        }
    }
}

void *handle_connection_task(void *pclientSocket){

    struct http_object *node_parser,get_http_content;
    node_parser = &get_http_content;

    int clientSocket = *((int*)pclientSocket); // cast to an int
    free(pclientSocket);
    char buffer[HEADER_BUFFER];
    size_t bytes_read;
    int msgsize = 0;

    //read the clients message
    while((bytes_read = read(node_parser->client_connect = clientSocket,buffer+msgsize,sizeof(buffer)-msgsize-1)) > 0){
        msgsize += bytes_read;
        if(msgsize > HEADER_BUFFER-1 || buffer[msgsize-1] == '\n')break;
    }
    buffer[msgsize-1] = 0; // null terminate message and remove the \n

     /* Read client request into buffer */
        //if(read(clientSocket,buffer,HEADER_BUFFER) < 0){ f_void_path_error(clientSocket); }
        
        /* Parse HTTP Header request  for GET/PUT - file of requested */
        char request[4],file[50],protocol[10];
        sscanf(buffer,"%s %s %s",request,file, protocol);
        //memset(buffer,0,sizeof(buffer));
        node_parser->buffer = buffer;
        node_parser->protocol = protocol;
        node_parser->request = request;
        node_parser->file_name = file;
        node_parser->client_connect = clientSocket;

        /* ------------------------REGEX SECTION ---------------------------- */

        bool matcha = f_regex_overkill_parse(node_parser->request,node_parser->file_name,node_parser->protocol);
        
        if(matcha != 1){ f_void_400(node_parser->client_connect); return NULL;}

        /*---------------------------------------------------------------------*/

        /* Get the file name - delimiter dash */
     
        char *theFile = node_parser->file_name;
        char *restOfString = theFile;
        char *file_name = strtok_r(restOfString,"/", &restOfString);
        node_parser->file_name = file_name;

        /* Check length of file must be = 10 ASCII */
        bool check_file_length = f_bool_check_file_len(node_parser->file_name);
      
        if(check_file_length == false){
            f_void_400(node_parser->client_connect);
            return NULL;
        }
      
        /* Checking valid resource names A-Z/a-z/0-9 */
        bool check_file_format = f_bool_check_file(node_parser->file_name);
        
        if(check_file_format == false){
            f_void_400(node_parser->client_connect);
            return NULL;
        } 
        if(strcmp(node_parser->request,"PUT") == 0){f_void_put_module(&get_http_content);return NULL;}
        return NULL;
}