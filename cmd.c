#include <string.h>
#include "utils.h"
#include <stdlib.h>

enum cmds_names{USERS = 0, QUIT, PRIVATE, PRIVATES, HELP, BAN, KICK, NICK, 
                SHUTDOWN,/* ROOM, TOPIC, SETADMIN, BANADMIN, UNBANADMIN, DELETE,
                PASSWD,*/ LAST};

struct one_cmd{
    char * cmd_str;
    void (*cmd_fun)(poll_fds *fds, clients *cl, int id, char * opt);
    char * help;
};

void users(poll_fds *fds, clients *cl, int id, char *opt);
void quit(poll_fds *fds, clients *cl, int id, char *opt);
void private(poll_fds *fds, clients *cl, int id, char *opt);
void privates(poll_fds *fds, clients *cl, int id, char *opt);
void help(poll_fds *fds, clients *cl, int id, char *opt);
void ban(poll_fds *fds, clients *cl, int id, char *opt);
void kick(poll_fds *fds, clients *cl, int id, char *opt);
void nick(poll_fds *fds, clients *cl, int id, char *opt);
void shutdown(poll_fds *fds, clients *cl, int id, char *opt);
/*
 * Нереализованный функционал
void room(poll_fds fds, clients cl, int id, char *opt);
void topic(poll_fds fds, clients cl, int id, char *opt);
void setadmin(poll_fds fds, clients cl, int id, char *opt);
void banadmin(poll_fds fds, clients cl, int id, char *opt);
void unbanadmin(poll_fds fds, clients cl, int id, char *opt);
void delete(poll_fds fds, clients cl, int id, char *opt);
void passwd(poll_fds fds, clients cl, int id, char *opt);
*/
struct one_cmd cmds_str[] = {
    {"\\users", users, " - выводит текущий список пользователей\n"},
    {"\\quit", quit, " [<сообщение>] - покинуть сервер с прощальным сообщением\n"},
    {"\\private", private, " <имя> - отправить приватное сообщение пользователю\n"},
    {"\\privates", privates, " - имена всех, кому вы отправляли приватные сообщения\n"},
    {"\\help", help, " - помощь по доступным командам\n"},
    {"\\ban", ban, " <имя> <причина> - заблокировать пользователя(А)\n"},
    {"\\kick", kick, " <имя> <причина> - отключить человека с сервера(А)\n"},
    {"\\nick", nick, " <старое имя> <новое имя> - поменять имя человеку(А)\n"},
    {"\\shutdown", shutdown, " <сообщение> - отключить сервер с сообщением(А)\n"},
/*  Нереализованный функционал:   
 *  {"\\room", room},
    {"\\topic", topic},
    {"\\setadmin", setadmin},
    {"\\banadmin", banadmin},
    {"\\unbamadmin", unbanadmin},
    {"\\delete", delete},
    {"\\passwd", passwd},
*/
};

int cmds(poll_fds *fds, clients *cl, int id, char * buf){
    int i;
    char * temp;
    for(i = 0; i < LAST - 1; i++){
        /* Да получается медленно, можно придумать какой-нибудь простой хэш */
        if(strncmp(buf, cmds_str[i].cmd_str, strlen(cmds_str[i].cmd_str)) == 0){
            temp = malloc(sizeof(char) * (strlen(buf) + 1));
            strcpy(temp, buf);
            cut(temp, strlen(cmds_str[i].cmd_str));
            if(temp[0] == ' ' || temp[0] == '\t' || temp[0] == '\0'){
                strip_beg(temp);
                cmds_str[i].cmd_fun(fds, cl, id, temp);
                free(temp);
                return 1;
            }
            else{
                free(temp);
                return 0;
            }
        }
    }
    return 0;
}

void users(poll_fds *fds, clients *cl, int id, char * opt){
    int i, len;
    char * temp, msg[] = "### Сейчас онлайн: ";
    len = sizeof(msg);
    for(i = 1; i < get_fds_size(); i++)
        len += strlen((*cl)[i].name) + 2;
    temp = malloc(sizeof(char) * len);
    strcpy(temp, "");
    strcat(temp, msg);
    for(i = 1; i < get_fds_size(); i++)
        if((*cl)[i].name[0] != '\0'){
            strcat(temp, (*cl)[i].name);
            if(i != get_fds_size() - 1)
                strcat(temp, ", ");
        }
    len -= 1;
    strcat(temp, "\n");
    ind_send(*fds, id, temp, len);
    free(temp);
}

void quit(poll_fds *fds, clients *cl, int id, char *opt){
    msg_everyone(*fds, *cl, id, opt); 
    disconnect(fds, cl, id);
}

void private(poll_fds *fds, clients *cl, int id, char *opt){
 

}
void privates(poll_fds *fds, clients *cl, int id, char *opt){

}
void help(poll_fds *fds, clients *cl, int id, char *opt){
    char promt[] = "### Команды: \n", *temp;
    int i, len = sizeof(promt);
    for(i = 0; i < LAST - 1; i++)
        len += strlen(cmds_str[i].help) + strlen(cmds_str[i].cmd_str);
    temp = malloc(sizeof(char) * len);
    strcpy(temp, promt);
    for(i = 0; i < LAST - 1; i++){
        strcat(temp, cmds_str[i].cmd_str);
        strcat(temp, cmds_str[i].help);
    }
    ind_send(*fds, id, temp, len);
    free(temp);
}
void ban(poll_fds *fds, clients *cl, int id, char *opt){
}
void kick(poll_fds *fds, clients *cl, int id, char *opt){
}
void nick(poll_fds *fds, clients *cl, int id, char *opt){
}
void shutdown(poll_fds *fds, clients *cl, int id, char *opt){

}
