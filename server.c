#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include "utils.h"

#define BUF_SIZE 256

int main(int argc, char * argv[]){
    int main_socket, port, events, temp_socket, i, clients_act;
    ssize_t n_read;
    char buf[BUF_SIZE];
    struct pollfd * fds; 
    clients clients_base;

    if(argc < 2){
        fprintf(stderr,"Необходимо указать номер порта в параметрах\n");
        return 1;
    }

    port = atoi(argv[1]);
     
    main_socket = init_socket(port);

    fds = init_fds();
    clients_base = init_clients();

    /*
    clients_info = malloc(sizeof(struct client_info) * (max_clients + 1));
    */
    if(fds == NULL){
        fprintf(stderr, "%s (%d): Структура не была создана: %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno));  
        exit(1);
    }
    
    clients_act = 0;
    fds = add_fds(fds, main_socket);

    for(;;){
        events = poll(fds, get_fds_size(), 100);
        if(events == -1){
            fprintf(stderr, "%s (%d): Проблемы с poll: %s\n",
                    __FILE__, __LINE__ - 3,  strerror(errno));  
            exit(1);
        }

        if(events == 0)
            continue;
        
        printf("Events = %d\n",events);

        if(fds[0].revents){
            temp_socket = accept(main_socket, NULL, NULL);
            if(temp_socket == -1){
                fprintf(stderr, "%s (%d): Не удалось принять: %s\n",
                        __FILE__, __LINE__ - 3,  strerror(errno));  
                exit(1);
            }
            clients_act++;
            printf("Клиент %d подсоединился\n", clients_act);
 
            fds = add_fds(fds, temp_socket);
            clients_base = add_client(clients_base); 
            

            write(temp_socket,"server-MZH\n",strlen("server-MZH\n"));
            auth(temp_socket);
            fds[0].revents = 0;
        }

        for(i = 1; i < get_fds_size(); i++){
            if(fds[i].revents){
                n_read = read(fds[i].fd, buf, BUF_SIZE);
                if(n_read == -1){
                    fprintf(stderr, "%s (%d): Ошибка при чтении из сокета: %s\n",
                            __FILE__, __LINE__ - 3,  strerror(errno));
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                if(n_read == 0){
                    printf("клиент %d отсоединился\n",i);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                if(n_read > 0){
                    buf[n_read] = '\0';
                    if(strcmp(clients_base[i].name, "\0") == 0)
                        auth2(clients_base, i, buf, fds[i].fd);
                    else{
                        strip(buf);
                        if(cmd(buf)){
                            /* Все сделается в cmd */;
                        }
                        else{
                            msg_everyone(fds, clients_base, i, buf); 
                        }
                
                    }
                }
       
            }
            fds[i].revents = 0;
        }
    }
    return 100;
}
