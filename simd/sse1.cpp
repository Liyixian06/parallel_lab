#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <immintrin.h>
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
	m_reset();
	gettimeofday(&start,NULL);
	for(int count=0;count<20;count++){
	for(int k=0;k<N;k++)
	{
		__m128 vt=_mm_set1_ps(m[k][k]);
		for(int j=k+1;j<N;j+=4){
			if(j+4>N)
				for(;j<N;j++)
					m[k][j]=m[k][j]/m[k][k];
			else{
				__m128 va=_mm_loadu_ps(m[k]+j); //alignment not required
				va=_mm_div_ps(va,vt);
				_mm_storeu_ps(m[k]+j,va);
			}
		}
		m[k][k]=1.0;
		for(int i=k+1;i<N;i++)
		{
			for(int j=k+1;j<N;j++)
				m[i][j]=m[i][j]-m[i][k]*m[k][j];
			m[i][k]=0;
		}
	}
	}
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time/20<<endl;
	return 0;	
}
