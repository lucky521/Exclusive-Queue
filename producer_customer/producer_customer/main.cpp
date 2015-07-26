#include <stdio.h>
#include <pthread.h> //多线程、互斥锁所需头文件
#include <semaphore.h> //线程信号量所需头文件

//设置生产者、消费者、缓冲区的个数
#define M 10
#define N 10
#define K 10
#define X 10

//互斥锁
pthread_mutex_t mutex;
//信号量
sem_t pro;
sem_t con;

//缓冲区和生产者、消费者的游标
int buffer[K] = {0};
int pro_idx = 0;
int con_idx = 0;

void *producer(void *)
{
    for(int i=0;i<X;i++)
    {
        sem_wait(&pro);
        pthread_mutex_lock(&mutex);
        
        int tmp = buffer[pro_idx];
        printf("Produce at buffer[%d] : from %d to 1\n", pro_idx, tmp);
        buffer[pro_idx] = 1;
        pro_idx = (pro_idx+1) % K;
    
        pthread_mutex_unlock(&mutex);
        sem_post(&con);
    }
    return NULL;
}

void *consumer(void *)
{
    for(int i=0;i<X;i++)
    {
        sem_wait(&con);
        pthread_mutex_lock(&mutex);
    
        int tmp = buffer[con_idx];
        printf("Consumer at buffer[%d] : from %d to 0\n", con_idx, tmp);
        buffer[con_idx] = 0;
        con_idx = (con_idx+1) % K;
    
        pthread_mutex_unlock(&mutex);
        sem_post(&pro);
    }
    return NULL;
}

int main(int argc, const char * argv[])
{
    //声明线程标识符
    pthread_t producer_thread[M];
    pthread_t consumer_thread[N];
    
    //初始化线程信号量：初始时为缓冲区全空
    sem_init(&pro, 0, K);
    sem_init(&con, 0, 0);
    
    //初始化互斥锁：下面初始化两种方式都可以
    mutex = PTHREAD_MUTEX_INITIALIZER;
    //pthread_mutex_init(&mutex, NULL);
    
    for (int i=0; i<M; i++)
    {
        if (pthread_create(&producer_thread[i], NULL, producer, NULL))
            printf("Producer Thread Cannot Created.\n");
    }
    
    for(int i=0;i<N;i++)
    {
        if(pthread_create(&consumer_thread[i], NULL, consumer, NULL))
            printf("Consumer Thread Cannot Created.\n");
    }
    
    for(int i=0;i<M;i++)
        pthread_join(producer_thread[i], NULL);
    for(int i=0;i<N;i++)
        pthread_join(consumer_thread[i], NULL);
    
    return 0;
}

