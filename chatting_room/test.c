/*************************************************************************
	> File Name: test.c
	> Author: WT
	> Mail: wangtong19951128@qq.com 
	> Created Time: Sat 08 Aug 2015 12:58:48 PM EDT
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>

void *
th1(void *arg)
{
    printf("0\n");
}

void *
th2(void *arg)
{
    printf("1\n");
}

int main(void)
{
    pthread_t tid1;
    while(1) {
        pthread_create(&tid1, NULL, th1, NULL);
        pthread_create(&tid1, NULL, th2, NULL);
    }


}
