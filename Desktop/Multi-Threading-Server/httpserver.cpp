/*
    Cristian C. Castillo & Baubak Saadat
    Professor Nawab
    UCSC CSE 130
    HTTPSERVER Assignment 2
*/

#include "httpserver_definitions.cpp"
#include <stdio.h> 
#include <stdlib.h> 

/* Function prototypes */
void create_worker(pthread_t * workerArr,int *thread,int numWorker);
void *handle_client_request(void * workerId);


int main(int argc, char **argv){
	// change back to nothing for client if malfunction of files

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
   
    /* Create socket */
    serverSocket = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(serverSocket < 0){ f_void_socket_error(serverSocket);}   

    /* Reuse socket addr*/
    if((ret=setsockopt(serverSocket, SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(int))) < 0){
        f_void_setsocket_error(ret);
    }
    
    /* Bind socket */
    if((ret = bind(serverSocket,res->ai_addr,res->ai_addrlen)) < 0 ){ f_void_bind_error(ret); }

    /* Initialize client queues, mutexes and conditions */
        
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&client_in_queue, NULL);

    /* Listen to sock & backlog */
    if((ret = listen(serverSocket,BACKLOG)) == -1){ f_void_listen_error(ret);}

    /* Marshall the struct - get rdy to pack msg */
    struct marshall *message = (struct marshall *)malloc(sizeof(marshall)); 
    
    if(cmd->Redundancy == true){message->is_redundant = cmd->Redundancy;}

   int clientSocket, flagSignal;
  
    int *threader = (int*)malloc(sizeof(int)*cmd->N_threads);
    pthread_t *dispatcher_thread = (pthread_t*)malloc(sizeof(pthread_t)*cmd->N_threads);
    create_worker(dispatcher_thread,threader,cmd->N_threads);
    sleep(5);
    printf("Hi there!\n");

    while(LIVE){
    	 //char data;
        //if(recv(clientSocket,&data,1,MSG_PEEK) <= 0){
        clientSocket = accept(serverSocket,(struct sockaddr *)&client_addr, &addr_size);
        /* Put that client to the queue, waiting for worker threads to handle it */
		pthread_mutex_lock(&mutex);
        {
            flagSignal = 0;
            /* There must be workers waiting, so send signal */
            if(items.empty())
                flagSignal = 1;
                
            items.push(clientSocket);

            if(flagSignal)
                pthread_cond_broadcast(&client_in_queue);
        }
		pthread_mutex_unlock(&mutex);
    
    }
    
}

void create_worker(pthread_t *workerArr, int *threadId_arr, int numWorker){
	int threadIndex;
	int returnVal;
	char message[100];

	for(threadIndex = 0; threadIndex < numWorker; threadIndex++){
		threadId_arr[threadIndex] = threadIndex;

		returnVal = pthread_create(&workerArr[threadIndex], NULL, worker_thread, &threadId_arr[threadIndex]);
		printf("Hi %d\n", threadIndex);
		if(returnVal){
			sprintf(message, "pthread_create() fail %d", returnVal); 
		}		
	}
    printf("I am finished\n");
    free(threadId_arr);    
    free(workerArr);	
}
