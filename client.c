#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>

#define BUF_SIZE 256
#define MAX_DESC 2

int lookup_host(const char *host, struct sockaddr_in * adr){
    struct addrinfo *res, *result;
    int errcode;

    errcode = getaddrinfo(host, NULL, NULL, &result);
    if (errcode != 0)
    {
        fprintf(stderr, "%s (%d): Введен неверный ip адрес : %s\n",
                __FILE__, __LINE__ - 4,  strerror(errno)); 
        freeaddrinfo(result);
        exit(1);
    }
      
    res = result;
    memcpy(adr, res->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(result);
    return 0;
}

int main(int argc, char * argv[]){
    int main_socket, port, events, i;
    ssize_t n_read;
    char buf[BUF_SIZE + 1];
    struct sockaddr_in adr;

    struct pollfd fds[MAX_DESC]; 

    if(argc < 3){
        fprintf(stderr,"Необходимо указать адрес и номер порта в параметрах\n");
        return 1;
    }

    if(argc > 3){
        fprintf(stderr,"Необходимо указать адрес и номер порта в параметрах\n");
        return 1;
    }

    port = atoi(argv[2]);
     
    lookup_host(argv[1], &adr);
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    /*
    if(inet_aton(argv[1], &(adr.sin_addr)) == -1){
         fprintf(stderr, "%s (%d): Введен неверный ip адрес : %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno)); 
        exit(1);
    }
    */
    printf("Connecting to %s (%s)...\n", argv[1], inet_ntoa(adr.sin_addr));
    

    main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket == -1){
        fprintf(stderr, "%s (%d): Сокет не был создан: %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno));  
        exit(1);
    }


    if(connect(main_socket, (struct sockaddr *) &adr, sizeof(adr)) == -1){
        fprintf(stderr, "%s (%d): Не удалось подсоединится: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }
    fds[0].fd = 0;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    fds[1].fd = main_socket;
    fds[1].events = POLLIN | POLLERR | POLLPRI;
    fds[1].revents = 0;

    for(;;){
        events = poll(fds, MAX_DESC, 100);
        if(events == -1){
            fprintf(stderr, "%s (%d): Проблемы с poll: %s\n",
                    __FILE__, __LINE__ - 3,  strerror(errno));  
            exit(1);
        }

        if(events == 0)
            continue;
        
        for(i = 0; i < MAX_DESC; i++){
            if(fds[i].revents > 0){
                n_read = read(fds[i].fd, buf, BUF_SIZE);
                if(n_read == -1){
                    fprintf(stderr, "%s (%d): Ошибка при чтении из сокета: %s\n",
                            __FILE__, __LINE__ - 3,  strerror(errno));
                    exit(1); /*Тк если один из вводов отвалился, то делать нечего*/
                }
                if(n_read == 0){
                    switch (i){
                        case 0:
                            /*Закончился ввод*/
                            exit(0);
                            break;
                        case 1:
                            /*Закончися сервер*/
                            fprintf(stderr, "Отключены от сервера\n");
                            exit(0);
                            break;
                    }
                }
                if(n_read > 0){
                    buf[n_read]='\0';
                    switch (i){
                        case 0:
                            write(main_socket, buf, n_read + 1);
                            break;
                        case 1:
                            write(1, buf, n_read + 1);
                            break;
                    }
                }
            }
            fds[i].revents = 0;
        }
    }
    return 100;
}
