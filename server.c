#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    int main_sock, client_sock, port = 3024, rc_len; /* порт по умолчанию */
    char buf[1024];

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;


    main_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    bind(main_sock, (struct sockaddr *)& addr, sizeof(addr));
    
    listen(main_sock, 5);
    for(;;){
        if((client_sock = accept(main_sock, NULL, NULL)) > 0){
            if(fork() == 0){
                for(;;){
                    rc_len = recv(client_sock, buf, sizeof(buf), 0);
                    if (rc_len == 0){
                        shutdown(client_sock, SHUT_RDWR);
                        close(client_sock);
                        exit(0);
                    }
                        
                    buf[rc_len] = '\0';
                    printf("Получена строка: %s", buf);
                
                }
            }
        }
    }

    return 0;
}
