#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
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
int main()
{
	struct timeval start;
	struct timeval end;
	m_reset();
	gettimeofday(&start,NULL);
	//common algo
	for(int count=1;count<=20;count++){
	for(int k=0;k<N;k++){
		for(int j=k+1;j<N;j++)
			m[k][j]=m[k][j]/m[k][k];
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
	float time = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
	cout<<time/20.0<<endl;
	return 0;
}
