/*Copyright © 2020- by Wangchuwen，2018202114. All rights reserved*/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include<semaphore.h>
#include <stdlib.h>

#define N 5
pthread_mutex_t chopsticks[N],pair[N];//实现筷子的互斥，和一双筷子的互斥
sem_t count;//信号量，用于控制哲学家数量
int timeline[5][4];//时间轴，用于可视化
int start;//记录时间起点
int k1,k2;//用于计算不同防止死锁办法的时间，比较效率

//方法一：将拿两只筷子变为一个原子操作。防止死锁
void * philosopher1(void * arg)
{
    int i = *((int *) arg);
        pthread_mutex_lock(&pair[i]);//原子操作
        pthread_mutex_unlock(&pair[(i+1)%N]);//防止右手边的人拿了筷子,右手边的人拿不了筷子，于是一双筷子就可以成为原子操作
        pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
        pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
        pthread_mutex_unlock(&pair[(i+1)%N]);
        pthread_mutex_unlock(&pair[i]);
        printf ("philosopher%d begins to eat.\n", i);
        timeline[i][0]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][1]=time((time_t*)NULL)-start;
        printf ("philosopher%d finished eating.\n", i);
;
        //释放左右两边的筷子
        pthread_mutex_unlock (&chopsticks[(i+1)%N]);
        pthread_mutex_unlock (&chopsticks[i]);
        printf ("philosopher%d begins to think.\n", i);
        timeline[i][2]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][3]=time((time_t*)NULL)-start;
        printf("philosopher%d finished thinking.\n", i);

}
//方法二：奇数和偶数哲学家拿筷子顺序相反，避免环，从而避免死锁
void * philosopher2(void * arg)
{
    int i = *((int *) arg);
    if (i % 2)
    {
        pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
        pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
        printf ("philosopher%d begins to eat.\n", i);
        timeline[i][0]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][1]=time((time_t*)NULL)-start;
        printf ("philosopher%d finished eating.\n", i);
        //释放左右两边的筷子
        pthread_mutex_unlock (&chopsticks[(i+1)%N]);
        pthread_mutex_unlock (&chopsticks[i]);
        printf ("philosopher%d begins to think.\n", i);
        timeline[i][2]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][3]=time((time_t*)NULL)-start;
        printf("philosopher%d finished thinking.\n", i);;
    }
    else
    {
        pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
        pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
        printf ("philosopher%d begins to eat.\n", i);
        timeline[i][0]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][1]=time((time_t*)NULL)-start;
        printf ("philosopher%d finished eating.\n", i);
        //释放在左右两边的筷子
        pthread_mutex_unlock (&chopsticks[i]);
        pthread_mutex_unlock (&chopsticks[(i+1)%N]);
        printf ("philosopher%d begins to think.\n", i);
        timeline[i][2]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][3]=time((time_t*)NULL)-start;
         printf("philosopher%d finished thinking.\n", i);
    }
}
//方法三：将资源从小到大编号，哲学家只能先拿编号大的筷子，即只有4号哲学家拿筷子顺序相反
void * philosopher3(void * arg)
{
    int i = *((int *) arg);
    if (i ==N-1)//控制资源请求顺序
    {
        pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
        pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
        printf ("philosopher%d begins to eat.\n", i);
        timeline[i][0]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][1]=time((time_t*)NULL)-start;
        printf ("philosN-1er%d finished eating.\n", i);
        //释放左右两边的筷子
        pthread_mutex_unlock (&chopsticks[(i+1)%N]);
        pthread_mutex_unlock (&chopsticks[i]);
        printf ("philosopher%d begins to think.\n", i);
        timeline[i][2]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][3]=time((time_t*)NULL)-start;
         printf("philosopher%d finished thinking.\n", i);
    }
    else
    {
        pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
        pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
        printf ("philosopher%d begins to eat.\n", i);
        timeline[i][0]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][1]=time((time_t*)NULL)-start;
        //释放在左右两边的筷子
        printf ("philosopher%d finished eating.\n", i);
        pthread_mutex_unlock (&chopsticks[i]);
        pthread_mutex_unlock (&chopsticks[(i+1)%N]);
        printf ("philosopher%d begins to think.\n", i);
        timeline[i][2]=time((time_t*)NULL)-start;
        sleep(3);
        timeline[i][3]=time((time_t*)NULL)-start;
         printf("philosopher%d finished thinking.\n", i);
    }
}
//方法四：只允许n-1个哲学家一起请求
void * philosopher4(void * arg)
{
    int i = *((int *) arg);
    sem_wait(&count);//控制哲学家数量
    pthread_mutex_lock (&chopsticks[i]);//获得左手的筷子
    pthread_mutex_lock (&chopsticks[(i+1)%N]);//获得右手的筷子
    
    printf ("philosopher%d begins to eat.\n", i);
    timeline[i][0]=time((time_t*)NULL)-start;
    sleep(10);
    timeline[i][1]=time((time_t*)NULL)-start;
    printf ("philosopher%d finished eating.\n", i);
    //释放左右两边的筷子
    pthread_mutex_unlock (&chopsticks[(i+1)%N]);
    pthread_mutex_unlock (&chopsticks[i]);
    sem_post(&count);
    printf ("philosopher%d begins to think.\n", i);
    timeline[i][2]=time((time_t*)NULL)-start;
    sleep(3);
    timeline[i][3]=time((time_t*)NULL)-start;
    printf ("philosopher%d finished thinking.\n", i);

}
//哲学家问题可视化模块
void graph()
{
    int k=0;
    int j=0;
    printf("过程时间轴可视化：\n");
    printf("E is eating,T is thinking!\n");
    for(;j<5;j++)
    {
        printf("philosopher%d: ",j);
        for(k=0;k<timeline[j][0];k++)
        {
            printf("-");//挨饿
        }
        for(;k<timeline[j][1];k++)
        {
            printf("E");//吃饭
        }
        for(;k<timeline[j][2];k++)
        {
            printf("-");//空闲
        }
        for(;k<timeline[j][3];k++)
        {
            printf("T");//思考
        }
        for(;k<k2-k1;k++)
        {
            printf("-");//空闲
        }
        printf("\n");
    }

}
//菜单设计
void menu()
{
    printf("-------------------MENU--------------------\n\n");
    printf("               1  只允许同时拿两只筷子          \n");
    printf("               2  相邻哲学家拿筷子顺序相反       \n");
    printf("               3  只能先拿筷子编号大的          \n");
    printf("               4  只允许n-1个哲学家就餐         \n");
    printf("               5  就餐结束，离席               \n\n");
    printf("-------------------------------------------\n");
    printf("请确定哲学家的就餐方式 option： ");

}
int main ()
{
    int i;
    pthread_t thread[N];
    int number[N];
    int error=sem_init(&count,0,4);//初始化信号量
    int option;//菜单选项
    if(error!=0)//信号量初始化检查
    {
        printf("sem_t init failed.\n");
        exit(1);
    }
    int rec[N];
    while (1)
    {
        menu();
        scanf("%d",&option);
        start=time((time_t*)NULL);
        switch (option)
        {
            case 1://方法一

                k1=time((time_t*)NULL);
                for (i = 0; i < N; i++)	//用一个线程来模拟一个哲学家
                {
                    number[i] = i;
                    rec[i]=pthread_create (&thread[i], NULL, philosopher1, &number[i]);
                    if(rec[i]!=0){
                        printf("thread phillosopher %d init failed.\n",i);
                        exit(1);
                    }
                }
                for ( i = 0; i < N; i++)
                {
                    pthread_join(thread[i], NULL);
                }
                k2=time((time_t*)NULL);
                printf( "就餐方式1，哲学家就餐共花费%d seconds\n", k2-k1 );
                graph();
                break;
            case 2://方法二

                k1=time((time_t*)NULL);
                for (i = 0; i < N; i++)	//用一个线程来模拟一个哲学家
                {
                    number[i] = i;
                    pthread_create (&thread[i], NULL, philosopher2, &number[i]);
                }
                for ( i = 0; i < N; i++)
                {
                    pthread_join(thread[i], NULL);
                }
                k2=time((time_t*)NULL);
                printf( "就餐方式2，哲学家就餐共花费%d seconds\n", k2-k1);
                graph();
                break; 
            case 3://方法三
                k1=time((time_t*)NULL);
                for (i = 0; i < N; i++)	//用一个线程来模拟一个哲学家
                {
                    number[i] = i;
                    pthread_create (&thread[i], NULL, philosopher3, &number[i]);
                }
                for ( i = 0; i < N; i++)
                {
                    pthread_join(thread[i], NULL);
                }
                k2=time((time_t*)NULL);
                printf( "就餐方式3，哲学家就餐共花费%d seconds\n", k2-k1);
                graph();
                break;
            case 4://方法四
                k1=time((time_t*)NULL);
                for (i = 0; i < N; i++)	//用一个线程来模拟一个哲学家
                {
                    number[i] = i;
                    pthread_create (&thread[i], NULL, philosopher4, &number[i]);
                }
                for ( i = 0; i < N; i++)
                {
                    pthread_join(thread[i], NULL);
                }
                k2=time((time_t*)NULL);
                printf( "就餐方式4，哲学家就餐共花费%d seconds\n", k2-k1);
                graph();
                break;
            case 5://退出
                printf("哲学家已离开......\n");
                return 0;
                break;
            default://特殊错误处理
                printf("sorry,wrong selection!\n");
                break;
            
        }
    }
   sem_destroy(&count); //信号量销毁

	return 0;
}

