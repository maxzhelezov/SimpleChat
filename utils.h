#ifndef UTILS_HEADER
#define UTILS_HEADER
/* Модуль с утилитами для сервера */

/* Максимальная очередь на подключение */
#define MAX_QUEUE 5
#define MEM_INC_SIZE 8
#define MAX_LEN 256

typedef struct pollfd * poll_fds;

typedef struct client_info {
    char name[256];
    int perm;
    int channel;
} * clients; 

/* Функция инициализирует серверный сокет */
int init_socket(int port);

/* Функиця аутенфикации - запрос имени у клиента */
void auth(int socket);

void auth2(clients cl, int client,  char * str, int socket);

/* Инциализацаия структуры клиентов */
clients init_clients();

/* Добавить клиента */
clients add_client(clients cl);

/* Очистка структуры клиентов */
void clean_clients(clients cl);

/* Создает структуру для полла и возращает на нее указатель */
poll_fds init_fds();

/* Добавляет дескриптор fd в структуру fds  */
poll_fds add_fds(poll_fds fds, int fd);

int get_fds_size();

/* Очищает структуру  fds*/
void clear_fds(poll_fds fds);

/* Очищает строку s от начальных пробельных символов, а также от \n в конце */
void strip(char * s);
        
/* Массовая рассылка сообщения s */
void mass_send(poll_fds fds, char *s, int size);

/* Пересылка особщения пользователя всем */
void msg_everyone(poll_fds fds, clients cl, int i, char * buf);

int cmd(char *buf);

#endif
