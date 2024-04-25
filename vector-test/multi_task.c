#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

//线程ID
pthread_t test01;
pthread_t test02;
pthread_t test03;

#define PTHREAD_STOPE   0               //线程停止状态
#define PTHREAD_RUNING  1               //线程运行状态

typedef void(*PTHREAD_RUN)(void *);

typedef struct testMuitl_pthread
{
    pthread_t pid;  //线程id
    int num;        //线程编号
    void *arg;      //参数
    int state;      //线程运行状态
    int sleepms;    //线程延时
    PTHREAD_RUN pthread_run;    //线程运行函数

}TESTMUITL_PTHREAD_T;
TESTMUITL_PTHREAD_T *pthread;  //线程结构


//获取当前时间
int GetCurTime(char *pTimeStr)
{
    time_t now_t = time(NULL);
    struct tm now = *gmtime(&now_t);
    now.tm_hour +=8;
    size_t len = strftime(pTimeStr, 40, "%F %T", &now);
    pTimeStr[len] = '\0';
    return 0;
}


//线程初始化
TESTMUITL_PTHREAD_T *Pthread_Create(int sleepSec,PTHREAD_RUN run,int num)
{
    TESTMUITL_PTHREAD_T *thread  = malloc(sizeof(*thread));

    if(thread != NULL)
    {
        memset(thread,0,sizeof(*thread));
        thread->pthread_run = run;
        thread->sleepms = sleepSec;
        thread->arg = thread;
        thread->num = num;
        thread->state = PTHREAD_STOPE;
    }

    return thread;
}

//线程启动
int Pthread_Start(TESTMUITL_PTHREAD_T *pthread,pthread_t pt_pid)
{
    int rtn = 0;

    if(pthread == NULL)
    {
        printf("Pthread_Start:pthread NULL.\n");
        return -1;
    }
    if(pthread->state == PTHREAD_RUNING)
    {
        printf("Pthread_Start:pthread is runing.\n");
        return -1;
    }

    if (pthread_create(&pt_pid, NULL, (void *)pthread->pthread_run, pthread->arg))
    {
        printf("Pthread_Start:pthread_create NULL\n");
        return -1;
    }
    pthread->state = PTHREAD_RUNING;

    return rtn;
}

//获取线程状态
int Pthread_Getstate(TESTMUITL_PTHREAD_T *pthread)
{
    return  pthread ? pthread->state : 0;
}


//线程运行函数
void Multi_PthreadFunction(void *arg)
{
    int rtn = 0;
    char tempbuf[50] = {0};

    TESTMUITL_PTHREAD_T *pthread = (TESTMUITL_PTHREAD_T *)arg;
    while(Pthread_Getstate(pthread) == PTHREAD_RUNING)
    {
        GetCurTime(tempbuf);
        printf("%s thread-0%d test!,thread_ID[%ld]\n ",tempbuf,pthread->num,pthread_self());
        sleep(pthread->sleepms);   //线程休眠
    }
}


/******************************************
*    函数名：Pthread_Function
*    功能:线程入口
*    入参：pthread_pid：当前线程Id
*         sleepSec：线程休眠时间（sec）
*         thread_num：测试线程编号
*    出参：
*    返回值：
********************************************/
void Pthread_Function(pthread_t pthread_pid,const int sleepSec,const int thread_num)
{

    pthread = Pthread_Create(sleepSec,Multi_PthreadFunction,thread_num);
    if(pthread == NULL)
    {
        printf("Pthread_Create failed.\n");
        return;
    }
    Pthread_Start(pthread,pthread_pid);         //启动线程
}


//test
int main()
{
    Pthread_Function(test01,1,1);   //3s
    Pthread_Function(test02,1,2);   //5s
    Pthread_Function(test03,1,3);   //8s

    while(1);
}

