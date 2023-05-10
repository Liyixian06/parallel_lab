#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <arm_neon.h>
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
pthread_barrier_t barrier_Division;
pthread_barrier_t barrier_Elimination;

void *threadFunc(void *param){
	threadParam_t *p = (threadParam_t*)param;
	int t_id = p->t_id;

	for(int k=0;k<N;++k){
		if(t_id==0){ //t_id为0的线程做除法操作，其他工作线程先等待
			float32x4_t vt=vmovq_n_f32(m[k][k]);
			for(int j=k+1;j<N;j+=4){
				if(j+4>N)
					for(;j<N;j++)
						m[k][j]=m[k][j]/m[k][k];
				else{
					float32x4_t va=vld1q_f32(m[k]+j);
					va=vmulq_f32(va,vt);
					vst1q_f32(m[k]+j,va);
				}
			}
			m[k][k]=1.0;
		}
		
		pthread_barrier_wait(&barrier_Division); //第一个同步点
		
		for(int i=k+1+t_id;i<N;i+=num_threads){ //循环划分任务
			for(int j=k+1;j<N;j+=4){
				if(j+4>N)
					for(;j<N;j++)
						m[i][j]=m[i][j]-m[i][k]*m[k][j];
				else{
					float32x4_t v1=vld1q_f32(m[i]+j);
					float32x4_t v2=vld1q_f32(m[i]+k);
					float32x4_t v3=vld1q_f32(m[k]+j);
					v2=vmulq_f32(v2,v3);
					v1=vsubq_f32(v1,v2);
					vst1q_f32(m[i]+j,v1);
				}
			}
			m[i][k]=0;
		}
		
		pthread_barrier_wait(&barrier_Elimination); //第二个同步点
	}
	
	pthread_exit(NULL);
	return;
}

int main(){
	struct timeval start;
	struct timebal end;
	m_reset();
	gettimeofday(&start,NULL);
	
	//初始化barrier
	pthread_barrier_init(&barrier_Division, NULL, num_threads);
	pthread_barrier_init(&barrier_Elimination, NULL, num_threads);
	
	//创建线程
	pthread_t* handles=new pthread_t[num_threads]; //创建handle
	threadParam_t* param=new threadParam_t[num_threads]; //创建线程数据结构
	for(int t_id=0;t_id<num_threads;t_id++){
		param[t_id].t_id=t_id;
		pthread_create(&handles[t_id],NULL,threadFunc,&param[t_id]);
	}

	for(int t_id=0;t_id<num_threads;t_id++)
		pthread_join(handles[t_id],NULL);
	
	//销毁barrier
	pthread_barrier_destroy(&barrier_Division);
	pthread_barrier_destroy(&barrier_Elimination);

	delete[] handles;
	delete[] param;
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}
