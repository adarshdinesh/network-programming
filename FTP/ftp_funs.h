#ifndef __FTP_FUNS__
#define __FTP_FUNS__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

enum {LIST, RETR, STOR, QUIT, UNKNOWN};

struct cmdtype {
    int     num;
    char    fname[100];
};

/*Error Reporting*/
void oops(char*);
#define oops(msg) {perror(msg);}

/*Global sock descriptor*/

int bindme(char *ip, int port){
    int     sockd;
    struct  sockaddr_in serv_addr;
    int     tr = 1;
    sockd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero ((char*) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr (ip);
    serv_addr.sin_port        = htons (port);
    
    if (setsockopt(sockd, SOL_SOCKET, SO_REUSEADDR, 
        &tr, sizeof(int) ) == -1) {
        oops("Setsockopt");
    }

    if (bind(sockd, (struct sockaddr *) &serv_addr,
        sizeof(struct sockaddr)) < 0 ){
        oops("Port Bind");
    }
    listen(sockd, 5);
    return sockd;
}


/*This module now uses subprocess 
and should be changed to use threads 
in the future*/

int connectme(int sockd){ 
    struct sockaddr_in cli_addr;
    int hostd;
    int clilen;
    clilen = sizeof(cli_addr);
    hostd = accept(sockd, (struct sockaddr *) &cli_addr, 
        &clilen);
    return hostd;
}


struct cmdtype parse(int hostd){
    struct cmdtype cmd; 
    memset(cmd.fname, '\00', 100);
    char buff[1024];
    int sz = read(hostd, buff, 1024);
    char cmdb[4];
    int i=0;
    for (i; i<sz-1; i++){
        if(i<4){
            cmdb[i] = buff[i];   
        }
        if(i>4){
            cmd.fname[i-5] = buff[i];
        }
    }
    cmdb[4]             = '\00';
    cmd.fname[sz-(4+1)] = '\00';
    if (!strcmp( cmdb, "LIST")) {
        cmd.num = LIST;
    }
    else if (!strcmp(cmdb, "STOR")) {
        cmd.num = STOR;
    }
    else if (!strcmp(cmdb, "QUIT")) {
        cmd.num = QUIT;
    }
    else if (!strcmp(cmdb, "RETR" )) {
        cmd.num = RETR;
    }
    else {
        cmd.num = UNKNOWN;
    }
    return cmd;
}

void banner(int hostd){
    send(hostd, "+-----------------------------------------+\n", 44, 0);
    send(hostd, "|            SIMPLE FTP SERVER            |\n", 44, 0);
    send(hostd, "+-----------------------------------------+\n", 44, 0);
    send(hostd, "| LIST | RETR <file> | STOR <file> | QUIT |\n", 44, 0);
    send(hostd, "+-----------------------------------------+\n", 44, 0);
}

void sendit(int hostd, char* chr){
    send(hostd, chr, strlen(chr), 0);    
}

void listit(int hostd){
    DIR *dp;
    char modi[1024];
    struct dirent *ep;    
    dp = opendir ("./");
    int i = 0;
    int j;
    if (dp != NULL)
    {
        while (ep = readdir (dp)){

            if( ep->d_type != DT_DIR && ep->d_name[0] != '.') {
                memset(modi, '\00', 1024);
                strcpy(modi, ep->d_name);
                fflush(stdout);
                modi[strlen(modi)]  = '\n';
                modi[strlen(modi)+1]= '\0';
                sendit(hostd,modi);
            }
        }
        closedir (dp);
        sendit(hostd, "\n");
    }
    else
        perror ("Couldn't open dir");                                                                                                                     
}
void cmdsend(int hostd, char* name){
    struct stat st;
    stat(name, &st);
    int size = st.st_size;
    char buff[1025];
    char pay[100];
    FILE *fd;
    int i=0;
    int j=0;
    char c;
    sprintf(pay, "SIZE{%d}\n",size);
    sendit(hostd, pay);
    size *= 2;
    fd = fopen(name,"rb");
    for (i=0; i < size/1024; i++){ 
           
        for (j=0; j < 1024; j+=2){
            c = fgetc(fd);
            sprintf(&buff[j], "%02x", c);
        }
        buff[1024]   = '\n';
        buff[1025] = '\00';
        sendit(hostd, buff);                             
    }

    for (j=0; j < size%1024; j+=2){
        c = fgetc(fd);
        sprintf(&buff[j], "%02x", c);
    }
    buff[size%1024]   = '\n';
    buff[size%1024+1] = '\00';
    sendit(hostd, buff);
}
void cmdrcv(int hostd, char* name){
    char modi[100];
    char buff[1024];
    char hex[2];
    int size;
    int i=0;
    FILE *fp;
    fp = fopen("amma","wb");
    int sz = read(hostd, modi, 100);
    sscanf(modi, "SIZE{%d}\n", &size );
    size *=2; 
    int k=0;
    unsigned long x;

    for (i=0; i < size/1024; i++){ 
            sz = read(hostd, buff, 1024);
            for(k=0;k<1024;k+=2){
                hex[0] = buff[k];
                hex[1] = buff[k+1];
                x = strtoul(hex, 0, 16);
                fputc((char)x, fp);
            }
    }

    sz = read(hostd, buff, size%1024);
    for(k=0;k<size%1024;k+=2){
        hex[0] = buff[k];
        hex[1] = buff[k+1];
        x = strtoul(hex, 0, 16);
        fputc((char)x, fp);
    }
    close(fp);

}
#endif

