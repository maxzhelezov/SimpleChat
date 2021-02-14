#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <poll.h>

#define MAX_QUEUE 5
#define MEM_INC_SIZE 8
#define BUF_SIZE 256

int main(int argc, char * argv[]){
    int main_socket, port, clients, max_clients, events, temp_socket, i;
    ssize_t n_read;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct pollfd * fds, * temp_fds; 

    if(argc < 2){
        fprintf(stderr,"Необходимо указать номер порта в параметрах\n");
        return 1;
    }

    port = atoi(argv[1]);
     
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = INADDR_ANY;

    errno = 0;
    main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket == -1){
        fprintf(stderr, "%s (%d): Сокет не был создан: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }

    errno = 0;
    if(bind(main_socket, (struct sockaddr *) &adr, sizeof(adr)) == -1){
        fprintf(stderr, "%s (%d): Не удалось привязать к адресу: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }
    
    errno = 0;
    if(listen(main_socket, MAX_QUEUE) == -1){
        fprintf(stderr, "%s (%d): Не удалось установить сокет в режим TCP: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }

    max_clients = MEM_INC_SIZE;
    clients = 0;

    errno =  0;
    fds = malloc(sizeof(struct pollfd) * (max_clients + 1));
    if(fds == NULL){
        fprintf(stderr, "%s (%d): Структура не была создана: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }

    fds[0].fd = main_socket;
    fds[0].events = POLLIN | POLLERR | POLLPRI | POLLOUT;
    fds[0].revents = 0;

    for(;;){
        events = poll(fds, clients + 1, 100);
        if(events == -1){
            fprintf(stderr, "%s (%d): Проблемы с poll: %s\n",
                    __FILE__, __LINE__ - 2,  strerror(errno));  
            exit(1);
        }

        if(events == 0)
            continue;
        
        printf("Events = %d\n",events);

        if(fds[0].revents){
            temp_socket = accept(main_socket, NULL, NULL);
            if(temp_socket == -1){
                fprintf(stderr, "%s (%d): Не удалось принять: %s\n",
                        __FILE__, __LINE__ - 2,  strerror(errno));  
                exit(1);
            }
            clients++;
            printf("Клиент %d подсоединился\n", clients);
            if(clients >= max_clients){
                max_clients += MEM_INC_SIZE;
                temp_fds = fds;
                fds = realloc(fds, sizeof(struct pollfd) * (max_clients + 1));
                if(fds == NULL){
                    fprintf(stderr, "%s (%d): Ошибка realloc: %s\n",
                            __FILE__, __LINE__ - 2,  strerror(errno));  
                    free(temp_fds);
                    exit(1);
                }
            }

            fds[clients].fd = temp_socket;
            fds[clients].events = POLLIN | POLLERR | POLLPRI | POLLHUP;
            fds[clients].revents = 0;
            write(temp_socket,"server-209 v0.0.1\n",strlen("server-209 v0.0.1\n"));
            //shutdown(temp_socket, SHUT_WR);
            fds[0].revents=0;
        }

        for(i = 1; i <= clients; i++){
            if(fds[i].revents){
                n_read = read(fds[i].fd, buf, BUF_SIZE);
                if(n_read == 0){
                    printf("клиент %d отсоединился\n",i);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                if(n_read == -1){
                    fprintf(stderr, "%s (%d): Ошибка при чтении из сокета: %s\n",
                            __FILE__, __LINE__ - 2,  strerror(errno));
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }
                if(n_read > 0){
                    buf[n_read]='\0';
                    printf("%d: %s\n", i, buf);
                }
       
            }
            fds[i].revents = 0;
        }
    }
    return 100;
}
