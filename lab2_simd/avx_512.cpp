#include <sys/time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
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
	struct timeval end;
	m_reset();
	gettimeofday(&start,NULL);
	for(int count=0;count<20;count++){
	for(int k=0;k<N;k++)
	{
		__m512 vt=_mm512_set1_ps(m[k][k]);
		for(int j=k+1;j<N;j+=16){
			if(j+16>N)
				for(;j<N;j++)
					m[k][j]=m[k][j]/m[k][k];
			else{
				__m512 va=_mm512_loadu_ps(m[k]+j); //alignment not required
				va=_mm512_div_ps(va,vt);
				_mm512_storeu_ps(m[k]+j,va);
			}
		}
		m[k][k]=1.0;
		for(int i=k+1;i<N;i++)
		{
			for(int j=k+1;j<N;j+=16){
				if(j+16>N)
					for(;j<N;j++)
						m[i][j]=m[i][j]-m[i][k]*m[k][j];
				else{
					__m512 v1=_mm512_loadu_ps(m[i]+j);
					__m512 v2=_mm512_loadu_ps(m[k]+j);
					__m512 v3=_mm512_set1_ps(m[i][k]);
					v2=_mm512_mul_ps(v2,v3);
					v1=_mm512_sub_ps(v1,v2);
					_mm512_storeu_ps(m[i]+j,v1);
				}
				m[i][k]=0;
			}
		}
	}
	}
	gettimeofday(&end,NULL);
	float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
	cout<<time/20<<endl;
	return 0;	
}
