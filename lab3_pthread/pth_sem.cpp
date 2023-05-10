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
sem_t sem_main;
sem_t sem_workerstart[num_threads];
sem_t sem_workerend[num_threads];

void *threadFunc(void *param){
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p->t_id;

	for(int k=0;k<N;++k){
		sem_wait(&sem_workerstart[t_id]); //阻塞，等待主线完成除法操作(操作自己专属的信号量)
		
		for(int i=k+1+t_id;i<N;i+=num_threads){
			for(int j=k+1;j<N;++j)
				m[i][j]=m[i][j]-m[i][k]*m[k][j];
			m[i][k]=0;
		}
		sem_post(&sem_main); //唤醒主线程
		sem_wait(&sem_workerend[t_id]); //阻塞，等待主线程唤醒进入下一轮
	}
	
	pthread_exit(NULL);
	return;
}

int main(){
	struct timeval start;
	struct timebal end;
	m_reset();
	gettimeofday(&start,NULL);
	
	sem_init(&sem_main,0,0); //初始化信号量
	for(int i=0;i<num_threads;++i){
		sem_init(&sem_workerstart[i],0,0);
		sem_init(&sem_workerend[i],0,0);
	}
	
	//创建线程
	pthread_t* handles=new pthread_t[num_threads]; //创建handle
	threadParam_t* param=new threadParam_t[num_threads]; //创建线程数据结构
	for(int t_id=0;t_id<num_threads;t_id++){
		param[t_id].t_id=t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,&param[t_id]);
	}

	for(int k=0;k<N;++k){
		for(int j=k+1;j<N;j++) //主线程做除法操作
			m[k][j] = m[k][j]/m[k][k];
		m[k][k]=1.0;
		
		//开始唤醒工作线程
		for(int t_id=0;t_id<num_threads;t_id++)
			sem_post(&sem_workerstart[t_id]);

		//主线程睡眠，等待所有工作线程完成此轮消去任务
		for(int t_id=0;t_id<num_threads;t_id++)
			sem_wait(&sem_main);

		//主线程再次唤醒工作线程，进入下一轮消去任务
		for(int t_id=0;t_id<num_threads;t_id++)
			sem_post(&sem_workerend[t_id]);
	}

	for(int t_id=0;t_id<num_threads;t_id++)
		pthread_join(handles[t_id],NULL);
	
	//销毁信号量
	sem_destroy(&sem_main);
	for(int t_id=0;t_id<num_threads;t_id++){
		sem_destroy(&sem_workerstart[t_id]);
		sem_destroy(&sem_workerend[t_id]);
	}

	delete[] handles;
	delete[] param;
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}
