/*************************************************************************
	> File Name: my_serve.c
	> Author: WT
	> Mail: wangtong19951128@qq.com 
	> Created Time: Sat 08 Aug 2015 09:45:26 AM EDT
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

#define PORT 80         //端口
#define MAXSIZE 1024    //输入与接收缓冲区
#define LISTEN 12       //监听最大数


struct account{
            char username[20];
            char passwd[10];
            char frdname[100][20];   //好友列表
}user[100];


struct data{
            int type;   //服务类型
            struct account info;  //账户信息
            char object_info[10][20];  //聊天对象信息，对象最多10人
            char chatting[256];     //聊天内容
            char ip[20];
}bag;


int fd[128];         //文件描述符集
char ip[128][20];    //ip列表


void 
save(struct account user)
{
    int fd;
    fd = open(user.username, O_WRONLY|O_CREAT|O_APPEND, 0700);
    write(fd , &user, sizeof(user));
    close(fd);
}


void 
read_f(struct account user)
{
    int fd;    
    fd = open(user.username, O_RDONLY|O_CREAT, 0700);
    read(fd, &user, sizeof(user));
    close(fd);
}


/**服务器初始读取用户数据**/
void 
init(void)
{
    int i = 0, fd, fd_u;
    char username[20];
    fd = open("account", O_RDONLY|O_CREAT, 0700);
    memset(user, 0, sizeof(user[100]));
    while(read(fd, username, 20)) {
        fd_u = open(username, O_RDONLY|O_CREAT, 0700);
        read(fd_u, &user[i++], sizeof(struct account));
        close(fd_u);
    }
}


void 
my_err(const char *err_string, int line)
{
    fprintf(stderr, "line:%d\n", line);
    perror(err_string);
    exit(0);
}


/**得到最大文件描述符**/
int 
get_max(void)
{
    int i, max = -1;
    for(i = 0; fd[i] != 0; i++) {
        if(fd[i] > max)
            max = fd[i];
    }
    return max;
}


/**查找数组中对应IP下标**/
int 
sea_ip(const char *string)
{
    int i;
    for(i = 0;; i++) {
        if(strcmp(string, ip[i]) == 0 )
            return i;
    }
}
void 
sys_reg(struct data memory)         //注册
{
    int i;
    const char *string = "账户创建成功";
    for(i = 0; user[i].username[0] != '\0'; i++);
        user[i] = memory.info;
       // printf("%s %s\n",user[i].username, user[i].passwd);   bug;三次输出;
    /*
     *重名处理
     *
     */
    strcpy(bag.chatting, string);
    i = sea_ip(memory.ip);
    send(fd[i], &bag, sizeof(bag), 0);
}


/**查找套接字描述符**/
int 
key(struct data memory)
{
    int i;
    for(i = 0; fd[i] != 0; i++) {
        if(strcmp(ip[i], memory.ip) == 0)
            return i;
    }
}

/**登录处理**/
void 
sys_log(struct data memory)
{
    const char *err_string = "帐号不存在";
    const char *string = "登录成功";
    const char *perr_string = "密码错误";
    int i, j, flag = 0;
    i = key(memory);
    for(j = 0; user[j].username[0] != '\0'; j++){
        if(strcmp(user[j].username, memory.info.username) == 0)
            flag = 1;
            break;
    }
    if(flag == 0){
        send(fd[i], err_string, strlen(string) + 1, 0);
        return;
    }
    
    if(strcmp(user[j].passwd, memory.info.passwd) == 0)
        send(fd[i], string, strlen(string) + 1, 0);
    else
        send(fd[i], perr_string, strlen(perr_string) + 1, 0);        
}


/***菜单***/
void 
branch(struct data memory)
{
    switch(memory.type){
    case 1:
        sys_reg(memory);
        break;
    case 2:
        sys_log(memory);
        break;
    case 3:
        /***/
        break;
    case 4:
        /***/
        break;
    }
}


void *
loop(void *arg)
{
    int i;
    fd_set reset;
    struct data memory;
    while(1) {
        FD_ZERO(&reset);
        for(i = 0; fd[i] != 0; i++)
            FD_SET(fd[i], &reset);
        select(get_max() + 1, &reset, NULL, NULL, NULL);
        for(i = 0; fd[i] != 0; i++)
             if(FD_ISSET(fd[i], &reset)) {
                recv(fd[i], &memory, sizeof(memory), 0); 
                 branch(memory);
        }
    }
}


int 
main(void)
{
    pthread_t tid;
    const char *string;
    int i = 0, j = 0, sock, sock_fd, option_value = 1, length = sizeof(option_value);
    int leng = sizeof(struct sockaddr);
    struct sockaddr_in sockaddr_ser, sockaddr_cli;

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        my_err("socket", __LINE__);
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option_value, length) < 0)
        my_err("setsockopt", __LINE__);
    
    memset(&sockaddr_ser, 0, sizeof(sockaddr_ser));
    sockaddr_ser.sin_family = AF_INET;
    sockaddr_ser.sin_port = htons(PORT);
    sockaddr_ser.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (struct sockaddr *)&sockaddr_ser, sizeof(struct sockaddr_in)) < 0)
        my_err("bind", __LINE__);
    listen(sock, LISTEN);
    memset(user, 0, sizeof(user));
    memset(fd, 0, 128);
    while(1) {
      if((sock_fd = accept(sock, (struct sockaddr *)&sockaddr_cli, &leng)) < 0) 
            my_err("accept", __LINE__);
        printf("accept a new client, ip:%s\n", inet_ntoa(sockaddr_cli.sin_addr));
        fd[i++] = sock_fd;
        string = inet_ntoa(sockaddr_cli.sin_addr);
        strcpy(ip[j++], string);
        send(sock_fd, string, strlen(string), 0);
        pthread_create(&tid, NULL, loop, NULL);
    }
}
