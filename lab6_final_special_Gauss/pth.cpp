#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<semaphore.h>
#include<sys/time.h>
#include<pthread.h>
using namespace std;
#define N 130
#define ELI 8
#define num_threads 4

struct line{
    bool upgrade;  //是否升级为消元子
    int first;  //首项
    bitset<N> bit;
};

bitset<N> R[N]; //消元子 R
line E[ELI]; //被消元行 E

typedef struct threadParam_t{
    int id;
};

pthread_barrier_t barrier_1;
pthread_barrier_t barrier_2;

void read_file()
{
    ifstream file;
    string num;

    //读消元子
    file.open("/home/ss2110577/Groebner/1_130_22_8/1.txt",ios_base::in); 
    while(getline(file,num))
    {
        bool flag = true;
        int i; //当前读入第i行
        stringstream st;
        int tmp; //每次读入一个数
        st<<num;
        while(st>>tmp)
        {
            if(flag) //设置首项i
            {
                i = tmp;
                flag = false;
            }
            R[i].set(tmp);
        }      
    }
    file.close();
    file.clear(ios::goodbit);

    //读被消元行
    file.open("/home/ss2110577/Groebner/1_130_22_8/2.txt",ios_base::in);
    int p = 0;
    while(getline(file,num))
    {
        bool flag = true;
        stringstream st;
        int tmp;
        st<<num;
        E[p].upgrade=false;
        while(st>>tmp)
        {
            if(flag) //设置首项
            {
                E[p].first=tmp;
                flag = false;
            }
            E[p].bit.set(tmp);
        }  
        p++;    
    }
    file.close();
}

int start;
void* threadFunc(void* param)
{
    threadParam_t* p = (threadParam_t*)param;
    int t_id = p->id;
    for(int i = N-1;i>=0;i--)
    {
        if(R[i].any()) //若消元子存在
        {
            for(int j=t_id;j<ELI;j+=num_threads)
            {
                if(E[j].first==i) //检查被消元行，若有对应首项就消元
                {
                    E[j].bit^=R[i];
                    if(E[j].bit.none()) //检查是否被消为全0，是则标记已升格
                    {
                        E[j].first=-1;
                        E[j].upgrade=true;
                    }
                    else
                    {
                        for (int l = E[j].first; l >= 0; l--) //更新被消元行的首项
                        {
                            if(E[j].bit[l] == 1)
                            {
                                E[j].first = l;
                                break;
                            }
                        }
                    } 
                }                                            
            }
        }
        else //若消元子不存在
        {
            pthread_barrier_wait(&barrier_1); //第一个同步点
            if(t_id==0) //t_id为0的线程执行升格操作
            {
                bool flag = true;
                for(int j =0;j<ELI;j++)
                {
                    if(E[j].first==i) //升格
                    {
                        R[i]=E[j].bit;
                        E[j].upgrade=true;
                        start=j;  //标记升格的被消元行
                        flag=false;
                        break;
                    }
                }
                if(flag)
                    start=ELI;
            }
            pthread_barrier_wait(&barrier_2); //第二个同步点，下面继续消元
            int new_start = t_id;
            for(;new_start<=start;new_start+=num_threads);
            for(int j = new_start;j<ELI;j+=num_threads)
            {
                if(E[j].first==i)
                {
                    E[j].bit^=R[i];
                    if(E[j].bit.none())
                    {
                        E[j].first=-1;
                        E[j].upgrade=true;
                    }
                    else
                    {
                        for (int l = E[j].first; l >= 0; l--)
                        {
                            if(E[j].bit[l] == 1)
                            {
                                E[j].first = l;
                                break;
                            }
                        }
                    }
                }                                      
            }
        }
    }
    pthread_exit(NULL);
    return;
}

int main()
{
    read_file();
    struct timeval start,end;
    gettimeofday(&start,NULL);
    
    //初始化barrier
    pthread_barrier_init(&barrier_1,NULL,num_threads);
    pthread_barrier_init(&barrier_2,NULL,num_threads);
    
    //创建线程
    pthread_t* handles = new  pthread_t[num_threads]; //创建handle
    threadParam_t* param = new threadParam_t[num_threads]; //创建线程数据结构

    for(int t_id = 0;t_id<num_threads;t_id++)
    {
        param[t_id].id = t_id;
        pthread_create((&handles[t_id]),NULL,threadFunc,(&param[t_id]));
    }    
    for(int t_id = 0;t_id<num_threads;t_id++)
        pthread_join(handles[t_id],NULL);

    //销毁barrier
    pthread_barrier_destroy(&barrier_1);
    pthread_barrier_destroy(&barrier_2);

    delete[] handles;
    delete[] param;
    gettimeofday(&end,NULL);
    float time = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
    cout<<time<<endl;
    return 0;
}


