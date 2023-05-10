#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;
const int N=200;
float m[N][N];
void m_reset()
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<i;j++)
			m[i][j]=0;
		m[i][i]=1.0;
		for(int j=i+1;j<N;j++)
			m[i][j]=rand();
	}
	for(int k=0;k<N;k++)
		for(int i=k+1;i<N;i++)
			for(int j=0;j<N;j++)
				m[i][j]+=m[k][j];
}

typedef struct threadParam_t{
	int t_id; //线程id
};

const int num_threads=8;
sem_t sem_leader;
sem_t sem_Division[num_threads-1];
sem_t sem_Elimination[num_threads-1];

void *threadFunc(void *param){
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p->t_id;

	for(int k=0;k<N;++k){
		if(t_id==0){ //t_id为0的线程做除法操作，其他工作线程先等待
			for(int j=k+1;j<N;j++)
				m[k][j]=m[k][j]/m[k][k];
			m[k][k]=1.0;
		}
		else sem_wait(&sem_Division[t_id-1]);
		
		if(t_id==0) // t_id为0的线程唤醒其他工作线程，进行消去操作
			for(int i=0;i<num_threads-1;++i)
				sem_post(&sem_Division[i]);
		
		for(int i=k+1+t_id;i<N;i+=num_threads){ //循环划分任务
			for(int j=k+1;j<N;++j)
				m[i][j]=m[i][j]-m[i][k]*m[k][j];
			m[i][k]=0;
		}
		
		if(t_id==0){
			for(int i=0;i<num_threads-1;i++)
				sem_wait(&sem_leader); //等待其它 worker 完成消去
			for(int i=0;i<num_threads-1;i++)
				sem_post(&sem_Elimination[i]); //通知其它 worker 进入下一轮
		}
		else{
			sem_post(&sem_leader); //通知 leader, 已完成消去任务
			sem_wait(&sem_Elimination[t_id-1]); // 等待通知，进入下一轮
		}
	}
	
	pthread_exit(NULL);
	return;
}

int main(){
	struct timeval start;
	struct timebal end;
	m_reset();
	gettimeofday(&start,NULL);
	
	sem_init(&sem_leader,0,0); //初始化信号量
	for(int i=0;i<num_threads-1;++i){
		sem_init(&sem_Division[i],0,0);
		sem_init(&sem_Elimination[i],0,0);
	}
	
	//创建线程
	pthread_t* handles=new pthread_t[num_threads]; //创建handle
	threadParam_t* param=new threadParam_t[num_threads]; //创建线程数据结构
	for(int t_id=0;t_id<num_threads;t_id++){
		param[t_id].t_id=t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,&param[t_id]);
	}

	for(int t_id=0;t_id<num_threads;t_id++)
		pthread_join(handles[t_id],NULL);
	
	//销毁信号量
	sem_destroy(&sem_leader);
	for(int t_id=0;t_id<num_threads-1;t_id++){
		sem_destroy(&sem_Division[t_id]);
		sem_destroy(&sem_Elimination[t_id]);
	}

	delete[] handles;
	delete[] param;
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}
