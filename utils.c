#include "utils.h"
#include <netinet/ip.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>

int max_fds, size_fds, del_fds, max_clients, size_clients;


/* Функция инициализирует серверный сокет */
int init_socket(int port){
    int main_socket;
    struct sockaddr_in adr;
    
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = INADDR_ANY;

    main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket == -1){
        fprintf(stderr, "%s (%d): Сокет не был создан: %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno));  
        exit(1);
    }

    if(bind(main_socket, (struct sockaddr *) &adr, sizeof(adr)) == -1){
        fprintf(stderr, "%s (%d): Не удалось привязать к адресу: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }
    
    if(listen(main_socket, MAX_QUEUE) == -1){
        fprintf(stderr, "%s (%d): Не удалось установить сокет в режим TCP: %s\n",
                __FILE__, __LINE__ - 2,  strerror(errno));  
        exit(1);
    }

    return main_socket;
}

poll_fds init_fds(){
    poll_fds temp;
    max_fds = MEM_INC_SIZE;
    size_fds = 0;
    del_fds = 0;
    temp = malloc(sizeof(struct pollfd) * max_fds);
    if(temp == NULL){
        fprintf(stderr, "%s (%d): Структура не была создана: %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno));  
        exit(1);
    }
    return temp;
}

poll_fds add_fds(poll_fds fds, int fd){
    poll_fds temp_fds;
    if(size_fds >= max_fds){
        max_fds += MEM_INC_SIZE;
        temp_fds = fds;
        fds = realloc(fds, sizeof(struct pollfd) * max_fds);
        if(fds == NULL){
            fprintf(stderr, "%s (%d): Ошибка realloc: %s\n",
                    __FILE__, __LINE__ - 3,  strerror(errno));  
            free(temp_fds);
            exit(1);
        }
    }
    fds[size_fds].fd = fd;
    fds[size_fds].events = POLLIN | POLLERR | POLLPRI;
    fds[size_fds].revents = 0;
    size_fds++;
    return fds;
}

int get_fds_size(){
    return size_fds;
}

int delete_fds(poll_fds *fds, int id){
    poll_fds temp;
    int i, j, new_id = id;
    (*fds)[id].fd = -1;
    del_fds++;
    /* Если мы так наудаляли на размер выделяемой памяти, то почистим массив 
     * вручную, да долго и муторно, но писать хэш-таблицу еще муторнее :(*/
    if(del_fds > MEM_INC_SIZE){
        max_fds = max_fds - MEM_INC_SIZE;
        if(max_fds < 1){
            fprintf(stderr, "%s (%d): Ошибка внутренней структуры данных\n",
                    __FILE__, __LINE__ - 3);  
            exit(1);
        }
        temp = malloc(sizeof(struct pollfd) * max_fds);
        if(temp == NULL){
            fprintf(stderr, "%s (%d): Ошибка выделения памяти malloc: %s\n",
                    __FILE__, __LINE__ - 3,  strerror(errno));  
            exit(1);
        }
        for(i = 0, j = 0; i < size_fds; i++){
            if((*fds)[i].fd != -1){
                temp[j] = (*fds)[i];
                if(id != -1)
                    new_id = j;
                j++;
            }
            /* Флаг, что прошли старый id, остожно портим его */
            if(id == i)
                id = -1;
        }
        size_fds = j;
        del_fds = 0;
        free(*fds);
        *fds = temp;
    }
    return new_id;

}

void delete_clients(clients cl, int id){
    return;
}

void clear_fds(poll_fds fds){
    free(fds);
    max_fds = 0;
    size_fds = 0;
}

void auth(int socket){
    char str[] = "### Введите свое имя: \n";
    write(socket, str, sizeof(str));
}

void auth2(clients cl, int client, char * str, int socket){
    char s[] = "### Добро пожаловать, ", * temp;
    char busy[] = "### Имя уже занято \n";
    int size, i;
    strip(str);
    if(strcmp(str, "") == 0)
        auth(socket);
    else{
        for(i = 1; i < size_clients; i ++)
            if(strcmp(str, cl[i].name) == 0){
                write(socket, busy, sizeof(busy));
                auth(socket);
                return;
            }


        strcpy(cl[client].name, str);
        size = sizeof(s) + (strlen(str) + 2) * sizeof(char);
        temp = malloc(size);
        strcpy(temp, "");
        strcat(temp, s);
        strcat(temp, str);
        strcat(temp, "!\n");
        write(socket, temp, size);
        free(temp);
    }        
}

int disconnect(poll_fds *fds, clients *cl, int id){
    int new_id;
    close((*fds)[id].fd);
    delete_clients(cl, id);
    new_id = delete_fds(fds, id);
    return new_id;
}

clients init_clients(){
    clients temp;
    size_clients = 1;
    max_clients = MEM_INC_SIZE;
    temp = malloc(sizeof(struct client_info) * max_clients);
    if(temp == NULL){
        fprintf(stderr, "%s (%d): Структура не была создана: %s\n",
                __FILE__, __LINE__ - 3,  strerror(errno));  
        exit(1);
    }
     return temp;
}

clients add_client(clients cl){
    clients temp;
    if(size_clients >= max_clients){
        max_clients += MEM_INC_SIZE;
        temp = cl;
        cl = realloc(cl, sizeof(struct client_info) * max_fds);
        if(cl == NULL){
            fprintf(stderr, "%s (%d): Ошибка realloc: %s\n",
                    __FILE__, __LINE__ - 3,  strerror(errno));  
            free(temp);
            exit(1);
        }
    }
    strcpy(cl[size_clients].name,"\0");
    cl[size_clients].perm = 0;
    cl[size_clients].channel = 0;
    size_clients++;
    return cl;
}

void strip(char * s){
    size_t pos;
    pos = strcspn(s, "\n");
    s[pos] = '\0';

    return;
}

void mass_send(poll_fds fds, char *s, int size){
    int i;
    for(i = 1; i < size_fds; i++)
        write(fds[i].fd, s, size);
}

int cmd(char * buf){
    return 0;
}

void msg_everyone(poll_fds fds, clients cl, int i, char *buf){
    char *temp;
    int size;
    size = strlen(buf) + strlen(cl[i].name) + 2 + 1;
    temp = malloc(size * sizeof(char));
    strcpy(temp, "");
    strcat(temp, cl[i].name);
    strcat(temp, ": ");
    strcat(temp, buf);
    strcat(temp, "\n");
    mass_send(fds, temp, size);
    free(temp);
}
