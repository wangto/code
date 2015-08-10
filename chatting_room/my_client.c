/*************************************************************************
	> File Name: my_client.c
	> Author: WT
	> Mail: wangtong19951128@qq.com 
	> Created Time: Sat 08 Aug 2015 03:41:29 PM EDT
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>

void sys_log(int sock_fd);

struct account{
            char username[20];
            char passwd[10];
};


struct data{
            int type;   //服务类型
            struct account info;  //账户信息
            char object_info[10][20];  //聊天对象信息，对象最多10人
            char chatting[256];     //聊天内容
            char ip[20];
}bag, bag_dup;


void 
my_err(const char *err_string, int line)
{
    fprintf(stderr, "line:%d\n", line);
    perror(err_string);
    exit(0);
}


void
private_chat(int sock_fd)
{



}


void 
group_chat(int sock_fd)
{
    bag.type = 4;


}


void
sys_reg(int sock_fd)
{
    bag.type = 1;
    printf("请输入用户名:");
    scanf("%s", bag.info.username);
    printf("请输入密码:");
    scanf("%s", bag.info.passwd);
    send(sock_fd, &bag, sizeof(bag), 0);
    recv(sock_fd, &bag_dup, sizeof(bag_dup), 0);
    printf("%s\n", bag_dup.chatting);
}

void
add_frd(int sock_fd)
{
    char choice, news[30];
    char username[30];
    while(1){
         printf("请输入用户名:")
         scanf("%s", username);
         send(sock_fd, username, sizeof(username), 0);
         recv(sock_fd, news, sizeof(news), 0);
         printf("%s\n", news);
         printf("是否继续y/n?");
         choice = getchar();
         if(choice == 'n')
            break;
    }
}



void 
menu_f(int sock_fd)
{
    while(1){
        int choice;
        printf("************************\n");
        printf("*\t1、注册      \t*\n");
        printf("*\t2、登录      \t*\n");
        printf("************************\n");
        printf("请选择:");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                    sys_reg(sock_fd);
                    break;
            case 2:
                    sys_log(sock_fd);
                     break;
            default:
                    printf("输入有误\n");
                    break;
        }
    }
}



void 
menu_s(int sock_fd)
{
    while(1){
        int choice;
        printf("*************************\n");
        printf("*\t1、私聊\t*\n");
        printf("*\t2、群聊\t*\n");
        printf("*\t3、添加好友\t*\n");
        printf("*************************\n");
        printf("请选择:");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                private_chat(sock_fd);
                break;
            case 2:
                group_chat(sock_fd);
                 break;
            case 3:add_frd(sock_fd);
                break;
        }
    }
}

void 
sys_log(int sock_fd)
{
    
    
    menu_s(sock_fd);
}

int 
main(void)
{
    int sock_fd; 
    struct sockaddr_in sockaddr_cli;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);    
    sockaddr_cli.sin_family = AF_INET;
    sockaddr_cli.sin_port = htons(80);
    inet_aton("192.168.1.121", &sockaddr_cli.sin_addr);
    if(connect(sock_fd, (struct sockaddr *)&sockaddr_cli, sizeof(struct sockaddr)) < 0)
        my_err("connect", __LINE__);
    recv(sock_fd, bag.ip, sizeof(bag.ip), 0);
    menu_f(sock_fd);
    




    return 0;
}
