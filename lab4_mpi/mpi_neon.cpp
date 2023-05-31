#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <arm_neon.h>
using namespace std;
const int N=1000;
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
	struct timeval end;

	int rank; //进程数
	int size; //当前是第几个进程
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//设置进程分配范围[r1,r2]
	int tasknum = N/size;
	int r1 = rank * tasknum;
	int r2;
	if(rank!=size-1)
		r2 = rank * tasknum + tasknum -1;
	else r2 = N-1;

	//0号进程初始化矩阵，发送到所有进程
	if (rank == 0) {
		m_reset();
		for (int i = 1; i < size; i++) {
            		MPI_Send(&m[0][0], N*N, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}
	}
	else MPI_Recv(&m[0][0], N*N, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	//块划分
	gettimeofday(&start,NULL);
    	for (int k = 0; k < N; k++) {
		if (k >= r1 && k <= r2) {
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
            		m[k][k] = 1.0;
			//将除法结果一对多广播给其他进程
            		for (int p = 0; p < size; p++) {
                			if (p != rank) {
                    			MPI_Send(&m[k][0], N, MPI_FLOAT, p, 0, MPI_COMM_WORLD);
                			}
            		}
        		}
		else {
            		MPI_Recv(&m[k][0], N, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); }
		for (int i = max(r1, k+1); i <=r2; i++) {
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
    	}
	MPI_Finalize();

	gettimeofday(&end,NULL);
    	float time = (end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time<<endl;
	return 0;
}

