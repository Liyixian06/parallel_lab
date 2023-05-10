#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
	int k; //消去的轮次
	int t_id; //线程id
};

const int worker_count=8; //工作线程数量

void *threadFunc(void *param){
	threadParam_t *p = (threadParam_t*)param;
	int k = p->k;
	int t_id = p->t_id;
	int i = k+t_id+1; //获取自己的计算任务

	for(int j=k+1;j<N;++j)
		m[i][j] = m[i][j]-m[i][k]*m[k][j];
	m[i][k]=0;
	pthread_exit(NULL);
	return;
}

int main(){
	struct timeval start;
	struct timebal end;
	m_reset();
	gettimeofday(&start,NULL);
	
	pthread_t* handles=new pthread_t[worker_count]; //创建handle
	threadParam_t* param=new threadParam_t[worker_count]; //创建线程数据结构

	for(int k=0;k<N;++k){
		for(int j=k+1;j<N;j++) //主线程做除法操作
			m[k][j] = m[k][j]/m[k][k];
		m[k][k]=1.0;
		
		//工作线程做消去操作
		for(int t_id=0;t_id<worker_count;t_id++){ //分配任务
			param[t_id].k=k;
			param[t_id].t_id=t_id;
		}
		for(int t_id=0;t_id<worker_count;t_id++) //创建线程
			pthread_create(&handles[t_id],NULL,threadFunc,&param[t_id]);
		for(int t_id=0;t_id<worker_count;t_id++) //主线程挂起，等待所有的工作线程完成此轮消去工作
			pthread_join(handles[t_id],NULL);
	}
	delete[] handles;
	delete[] param;
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}
