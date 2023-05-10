#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
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
int main(){
	struct timeval start;
	struct timeval end;//clock
	m_reset();
	gettimeofday(&start,NULL);
	const int num_threads=8;
	int i,j,k;
	float32x4_t vt,va,v1,v2,v3;

//在外循环之外创建线程，避免线程反复创建销毁
#pragma omp parallel if(1), num_threads(num_threads), private(i, j, k, vt, va, v1, v2, v3)
	for(k=0;k<N;k++){
#pragma omp single //串行部分
		vt=vmovq_n_f32(m[k][k]);
		for(int j=k+1;j<N;j+=4){
			if(j+4>N)
				for(;j<N;j++)
					m[k][j]=m[k][j]/m[k][k];
			else{
				va=vld1q_f32(m[k]+j);
				va=vmulq_f32(va,vt);
				vst1q_f32(m[k]+j,va);
			}
		}
		m[k][k]=1.0;
#pragma omp for //并行部分
		for(int i=k+1;i<N;i++)
		{
			for(int j=k+1;j<N;j+=4){
				if(j+4>N)
					for(;j<N;j++)
						m[i][j]=m[i][j]-m[i][k]*m[k][j];
				else{
					v1=vld1q_f32(m[i]+j);
					v2=vld1q_f32(m[i]+k);
					v3=vld1q_f32(m[k]+j);
					v2=vmulq_f32(v2,v3);
					v1=vsubq_f32(v1,v2);
					vst1q_f32(m[i]+j,v1);
				}
			}
			m[i][k]=0;
		}
	//离开for循环时，各个线程默认同步，进入下一行的处理
	}

	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}
