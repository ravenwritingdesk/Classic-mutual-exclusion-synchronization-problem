/*Copyright © 2020- by Wangchuwen，2018202114. All rights reserved*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
//生产者数量，消费者数量，缓冲区buffer大小
#define PN 3
#define CN 3
#define BN 5
//信号量，用来检测缓冲区的占用情况
sem_t bufferempty;
sem_t bufferfull;
//互斥量，同一时间只有一个生产者/消费者可以访问buffer
pthread_mutex_t door;
int producer_id=0;
int consumer_id=0;
int pro_set=0;
int con_set=0;
int start;//排队计时器
int now;
int wait;//等待时间
//初始化buffer
int buffer[BN]={0};
char *production[10] = {"大波纹薯条","香辣鸡腿堡","吮指原味鸡","葡式蛋挞-","嫩牛五方-","香辣鸡翅-","香甜玉米棒","醇香土豆泥","小食拼盘-","现烤可颂-"};
//可视化
void mvisual()
{
    printf("KFC前台：\n");
    for(int i=0;i<BN;i++)
    {
            printf("  %d号窗口   ",i);
    }
    printf("\n");
    for(int i=0;i<BN;i++)
    {
        if(buffer[i]==0)
        {
           printf("   请等待   ");
        }
        else 
        {
            printf(" %s ",production[buffer[con_set]]);
        }
            
    } 
    printf("\n");
}
//生产者
void *producer()
{
	int id=++producer_id;//每次加入一个producer，id加一
	while(wait>now-start){//是否到达等待时间
		sleep(1);
		sem_wait(&bufferempty);//等待有空的位置
		pthread_mutex_lock(&door);//等待其他进程
		pro_set=pro_set%BN;//实现循环buffer
		printf("%s已经做好，请到%d号窗口取餐\n",production[id],pro_set);
		buffer[pro_set]=id;
		mvisual();
		++pro_set;
		pthread_mutex_unlock(&door);//允许其他进程访问buffer
		sem_post(&bufferfull);//填充程度加一
        now=time((time_t*)NULL); 
	}
}


void *consumer()
{
    int id = ++consumer_id;//每次加入一个producer，id加一 
	while(wait>now-start){//是否到达等待时间
		sleep(1);
		sem_wait(&bufferfull);//等待有填充
		pthread_mutex_lock(&door);//等待其他进程
		con_set=con_set%BN;
		printf("顾客%d已取走在%d窗口的%s.\n", id,con_set,production[buffer[con_set]]);
		buffer[con_set]=0;
		mvisual();
		++con_set;
		pthread_mutex_unlock(&door);//允许其他进程访问buffer
		sem_post(&bufferempty);//空闲程度加一
        now=time((time_t*)NULL); 				
	}

}


//菜单设计
void menu()
{
    printf("欢迎来到KFC！🍗\n");
    printf("----------------------MENU----------------------\n\n");
    printf("                   1   查看排队状态                 \n");
    printf("                   2   查看菜单                    \n");
    printf("                   3   离开                   \n\n");
    printf("------------------------------------------------\n");
    printf("option: ");

}


int main()
{
    int option;
    while(1)
    {
        menu();
        scanf("%d",&option);
        switch (option)
        {
        case 1:
            printf("请输入您的等待时间:(建议输小点，这样很快结束，如8) ");
            scanf("%d",&wait);
            start=time((time_t*)NULL);
             //设置生产者消费者进程
            pthread_t xproducer[PN];
            pthread_t xconsumer[CN];
            //初始化信号量
            int init_p=sem_init(&bufferempty,0,BN);
            int init_c=sem_init(&bufferfull,0,0);
            if(init_p&init_c!=0){
                printf("sem init error\n");
                exit(1);
            }
            //初始化互斥量
            int init_d=pthread_mutex_init(&door,NULL);
            if(init_d)
            {
                printf("mutex init error\n");
                exit(1);
            }
            int i;
            int ret_p[PN];
            int ret_c[CN];
            for(i=0;i<PN;i++){// 创建PN个producer线程
                ret_p[i] = pthread_create(&xproducer[i],NULL,producer,(void*)(&i));
                if(ret_p[i]!=0){
                        printf("producer_%d creation failed.\n",i);
                        exit(1);
                }
            }

            for(i=0;i<CN;i++){//创建CN个consumer线程 
                ret_c[i]=pthread_create(&xconsumer[i],NULL,consumer,NULL);
                if(ret_c[i]!=0){
                        printf("consumer_%d creation failed.\n",i);
                        exit(1);
                }
            }

            for(i=0;i<PN;i++){//销毁线程
                pthread_join(xproducer[i],NULL);//以阻塞的方式等待指定的线程结束
            }
            for(i=0;i<CN;i++){//销毁线程
                pthread_join(xconsumer[i],NULL);
            }
            break;
        case 2://菜单设计
            printf("大波纹薯条 $4\n香辣鸡腿堡 $7\n吮指原味鸡 $5\n葡式蛋挞  $3\n嫩牛五方  $7\n香辣鸡翅  $5\n香甜玉米棒 $2\n醇香土豆泥 $2\n小食拼盘  $9\n现烤可颂  $3\n");
            break;
        case 3:
            printf("欢迎下次光临👏\n");
            return 0;
            break;
        default:
            printf("sorry,worry selection");
            break;
        }
       
    }
    return 0;
}





