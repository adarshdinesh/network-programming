/********************************************************/
/*  	SIMPLE MULTI THREADED SERVER BY ADU               */ 
/********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>


void oops(char* msg);
#define oops(msg) { perror(msg); exit(-1); }
void oops_b(char* msg);
#define oops_b(msg) { perror(msg); break; }

#define _MAX_ 100
#define SERVER_PORT 1337

int server;

void clean(){
        close(server);
      	pthread_exit(NULL);
        return;
}

int readnf (int fd, char *line){
        if (read(fd, line, _MAX_) < 0)
                return 1; 
        return 0;
} 

void banner(){
        fprintf(stdout, "**********SIMPLE THREADED SERVER**********\n");
        fflush(stdout);
        return;
}
int strip(char* str, int* size){ //strip() function
        str[*size-1] = '\x00';
        (*size) = (*size)-1;
}
int parse(char *str, int n){
        //parser 
}
void handler(void* clsd){
        char line[_MAX_]; 
        struct sockaddr_in cliadd;
        int addr_len, client_loc, command;
        int size;
        addr_len = sizeof(cliadd);
        client_loc = *((int*) clsd);
  	getpeername(client_loc, (struct sockaddr*)&cliadd, &addr_len);	
        printf("Connected \n");
        bzero(line,100);
        while(readnf(client_loc, line)!=1) {
                size = strlen(line);
                if (size == 0)
                        break;
                strip(line, &size);
                command = parse(line, size);
                printf("old:%d\nnew:%d\n",size,strlen(line));
                bzero(line,100);
        }       
        close(client_loc);
        return;

}
int main(void){
        int client;
        int addr_len;
        pthread_t thread;
	
        struct sockaddr_in cli_addr;
        struct sockaddr_in serv_addr;

        signal(SIGINT,clean);
        signal(SIGTERM,clean);
        signal(SIGTSTP,clean);	
	
        server = socket(PF_INET, SOCK_STREAM, 0);
        if (server < 0){
                oops("Socket error :( ");
        }
        else {
                banner();
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(SERVER_PORT);
	
        memset(serv_addr.sin_zero, 0, 8);
        if (bind(server, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr))< 0)
                oops("Bind :");	
        listen(server, 5);
	
        while (1){
                addr_len = sizeof(cli_addr);
                client = accept(server, (struct sockaddr *) &cli_addr, &addr_len);
                if (client < 0)
                        oops_b("Accept :");
                pthread_create(&thread, 0, (void*)&handler, (void*)&client);


        }
        close(server);
        exit(0);
}
