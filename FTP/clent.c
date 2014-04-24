#include <stdio.h>
#include "ftp_funs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY (key_t)7172

struct goaway{
    int cmd;
    int flag;
    char file[100];
};

int main(){
    struct cmdtype cmds;
    int sockd;
    int id; 
    id = fork();
    if(id == 0) { /*cmd line*/
        int shm = shmget(KEY, sizeof(struct goaway), IPC_CREAT|0644);
        struct goaway *foo = shmat(shm, NULL, 0);
        do{
            sockd = connectme2("127.0.0.1", 1338);
            foo->cmd   = -1;
            foo->flag  = -1;

            do{

                cmds = parse(hostd);
                printf("%d", cmds.num);
                printf("%s", cmds.fname);
                switch(cmds.num){
                    case 0: /*LIST*/
                            foo->cmd = cmds.num;
                            foo->flag = 1;                   
                            sendit(hostd, "<LIST OK>\n");
                            break;
                    case 1: /*RETR*/
                            foo->cmd = cmds.num;
                            foo->flag = 1;
                            strcpy(foo->file, cmds.fname); 
                            sendit(hostd, "<RETR OK>\n");                        
                            break;
                    case 2: /*STOR*/
                            foo->cmd = cmds.num;
                            foo->flag = 1;
                            strcpy(foo->file, cmds.fname); 
                            sendit(hostd, "<STOR OK>\n");
                            break;
                    case 3: /*QUIT*/
                            foo->cmd = cmds.num;
                            sendit(hostd, "<BYE>\n");
                            break;
                    default: /*UNKNOWN*/
                            
                            sendit(hostd, "<ERROR>\n");
                       
               };
                fflush(stdout);
            }while(cmds.num!=QUIT);    
            close(hostd);
        }while(1);
        close(sockd); /*Not reached .
        Have to do something on this*/
    }
    else { /*CLI*/
        int shm = shmget(KEY, sizeof(struct goaway), IPC_CREAT|0644);
        struct goaway *foo = shmat(shm, NULL, 0);
        char* bar;
        do {
            hostd = connectme2(sockd);
            foo->cmd  = -1;
            foo->flag = -1;
            do {            
                    printf("\n>");
                    scanf("%[^\n]",bar);

                    if (!strcmp( bar, "LIST")) {
                        foo->cmd = LIST;
                    }
                    else if (!strcmp(cmdb, "STOR")) {
                        foo->cmd = STOR;
                        foo->
                    }
                    else if (!strcmp(cmdb, "QUIT")) {
                        foo->cmd = QUIT;
                    }
                                                                                else if (!strcmp(cmdb, "RETR" )) {
                                                                                            cmd.num = RETR;
                                                                                                }
                                                                                                    else {
                                                                                                                cmd.num = UNKNOWN;
                                                                                                                    }

                }
            }while(foo->cmd != QUIT);
            close(hostd);
        }while(1); 
        close(sockd);/*Not reached .
        Have to do something on this*/
        wait();                                                  
    }
    return 0;
}
