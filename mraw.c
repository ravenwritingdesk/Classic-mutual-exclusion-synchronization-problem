/*Copyright © 2020- by Wangchuwen，2018202114. All rights reserved*/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include<semaphore.h>
#include <stdlib.h>
//semaphores
sem_t mutex1,mutex2,mutex3,mutex4;//用于原子操作
sem_t mread,mwrite,mRead,mWrite;//用于读写互斥
int writecount,readcount,Writecount,Readcount;//用于计数读写者数量

void * Reader(void *counter);//读者优先
void * writer(void *counter);

void * reader(void *counter);//写者优先
void * Writer(void *counter);
int a,b;//读写者数量
int start;//计时器开始，便于可视化
int *rtime,*wtime;//用于计时
//可视化
void visual()
{
    int k=0;
    int j=0;
    printf("过程时间轴可视化：\n");
    printf("R is reading,W is writing!\n");
    for(j=0;j<a;j++)
    {
        printf("READER%d: ",j);
        for(k=0;k<rtime[j];k++)
        {
            printf("-");
        }
        for(;k<rtime[j]+2;k++)
        {
            printf("R");
        }
        printf("\n");
    }
    for(j=0;j<b;j++)
    {
        printf("WRITER%d: ",j);
        for(k=0;k<wtime[j];k++)
        {
            printf("-");
        }
        for(;k<wtime[j]+2;k++)
        {
            printf("W");
        }
        printf("\n");
    }
    printf("\n");

}
//菜单设计
void menu()
{
    printf("-------------------MENU--------------------\n\n");
    printf("               1  读者优先                   \n");
    printf("               2  写者优先                   \n");
    printf("               3  退出               \n\n");
    printf("-------------------------------------------\n");
    printf(" option： ");
}

/*读者优先*/
void *Reader(void * counter){
    int id = *((int*)counter);
    sleep(1);
    printf("READER %d: waiting to read\n", id);
    sem_wait(&mutex4);//用于互斥操作readcount
    Readcount++;
    if(Readcount == 1) 
        sem_wait(&mwrite);//不允许写
    sem_post(&mutex4);

    printf("READER %d: start reading\n", id);
    rtime[id]=time((time_t*)NULL)-start;//用于记录时间点
    sleep(2);//模拟read过程
    printf("READER %d: end reading\n", id);

    sem_wait(&mutex4);//用于互斥操作readcount
    Readcount--;
    if(Readcount == 0)
        sem_post(&mwrite);//无读进程等待，允许写
    sem_post(&mutex4);

}
void *writer(void *counter){
    int id = *((int*)counter);
    
    sleep(1);
    printf("WRITER %d: waiting to write\n", id);

    sem_wait(&mwrite);//实现写者互斥
    printf("WRITER %d: start writing\n", id);
    wtime[id]=time((time_t*)NULL)-start;
    sleep(2);
    printf("WRITER %d: end writing\n", id);
    sem_post(&mwrite);


}

/*写者优先*/

void* reader(void* counter) {
	int id = *((int*)counter);
    sleep(1);
	printf("READER %d: waiting to read\n", id);	
    
    sem_wait(&mutex1);//防止对mread wait多次
    sem_wait(&mread);//等待没有写进程排队再读
	sem_wait(&mutex2);//互斥操作readcount
	readcount++;
	if(readcount == 1)
		sem_wait(&mWrite);
	sem_post(&mutex2);
	sem_post(&mread);
    sem_post(&mutex1);

	printf("READER %d: start reading\n", id);
    rtime[id]=time((time_t*)NULL)-start;//记录时间点
    sleep(2);//模拟read
	printf("READER %d: end reading\n", id);

	sem_wait(&mutex2);//互斥操作readcount
	readcount--;
	if(readcount == 0)
		sem_post(&mWrite);
	sem_post(&mutex2);


}

void* Writer(void* counter) {
	int id = *((int*)counter);
    sleep(1);
	printf("WRITER %d: waiting to write\n", id);
	
	sem_wait(&mutex3);//互斥操作writecount
	Writecount++;
	if(Writecount == 1){
		sem_wait(&mread);//不允许读
	}
	sem_post(&mutex3);
	
	sem_wait(&mWrite);//写者互斥
	printf("WRITER %d: start writing\n", id);
    wtime[id]=time((time_t*)NULL)-start;
    sleep(2);//模拟write
	printf("WRITER %d: end writing\n", id);
	sem_post(&mWrite);

	sem_wait(&mutex3);//互斥操作writecount
	Writecount--;
	if(Writecount == 0) {
		sem_post(&mread);//当无写者排队时，允许读者读
	}
	sem_post(&mutex3);
	
}

int main() {

        int i;
        //初始化信号量
        sem_init(&mutex1, 0, 1);
        sem_init(&mutex2, 0, 1);
        sem_init(&mutex3, 0, 1);
        sem_init(&mutex4, 0, 1);
        sem_init(&mwrite, 0, 1);
        sem_init(&mread, 0, 1);
        sem_init(&mWrite, 0, 1);
        sem_init(&mRead, 0, 1);
        //选项
        int option;
        readcount = writecount = Writecount = Readcount = 0;
        //线程
        pthread_t *preader,*pwriter;
        //读写者
        int *readerid,*writerid;
        //错误检查
        int *retr,*retw;


	 while(1)
    {
        start=time((time_t*)NULL); 
        printf("请输入读者和写者的数量，用空格隔开: ");
        scanf("%d %d",&a,&b);
        //分配动态内存
        readerid=malloc(sizeof(int) * a);
        writerid=malloc(sizeof(int) * b);
        retr=malloc(sizeof(int) * a);
        retw=malloc(sizeof(int) * b); 
        preader=malloc(sizeof(pthread_t) * a);
        pwriter=malloc(sizeof(pthread_t) * b);
        rtime=malloc(sizeof(int) * a);
        wtime=malloc(sizeof(int) * b); 
        //菜单
        menu();
        scanf("%d",&option);
        switch (option)
        {
        case 1://读者优先
        //创建线程
            for (i = 0; i < a; i++)	
            {
                readerid[i] = i;
                retr[i]=pthread_create (&preader[i], NULL, Reader, &readerid[i]);
                if(retr[i]!=0){
                    printf("Reader %d init failed.\n",i);
                    exit(1);
                }
            }
            for (i = 0; i < b; i++)	
            {
                writerid[i] = i;
                retw[i]=pthread_create (&pwriter[i], NULL, writer, &writerid[i]);
                if(retw[i]!=0){
                    printf("Writer %d init failed.\n",i);
                    exit(1);
                }
            }
            //结束线程
            for (i = 0; i < a; i++)
            {
                pthread_join(preader[i], NULL);
            }
            for (i = 0; i < b; i++)
            {
                pthread_join(pwriter[i], NULL);
            }
            visual();
            break;
        case 2://写者优先
        //创建线程
            for (i = 0; i < a; i++)	
            {
                readerid[i] = i;
                retr[i]=pthread_create (&preader[i], NULL, reader, &readerid[i]);
                if(retr[i]!=0){
                    printf("Reader %d init failed.\n",i);
                    exit(1);
                }
            }
            for (i = 0; i < b; i++)	
            {
                writerid[i] = i;
                retw[i]=pthread_create (&pwriter[i], NULL, Writer, &writerid[i]);
                if(retw[i]!=0){
                    printf("Writer %d init failed.\n",i);
                    exit(1);
                }
            }
            //结束线程
            for (i = 0; i < a; i++)
            {
                pthread_join(preader[i], NULL);
            }
            for (i = 0; i < b; i++)
            {
                pthread_join(pwriter[i], NULL);
            }
            //可视化
            visual();
            break;
        case 3://退出
            printf("成功退出\n");
            return 0;
            break;
        default:
            printf("sorry,worry selection!");
            break;
        }

    }
//销毁信号量
	sem_destroy(&mutex1);
	sem_destroy(&mutex2);
	sem_destroy(&mutex3);
    sem_destroy(&mutex4);
    sem_destroy(&mread);
    sem_destroy(&mRead);
    sem_destroy(&mwrite);
    sem_destroy(&mWrite);



	return 0;
}






