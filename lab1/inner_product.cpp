#include <iostream>
#include <windows.h>

using namespace std;

void common(){
    cout<<"This is common algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for(int i=1;i<10;i++){//n=10-90
        time=0;
        const int n=10*i;
        int num[n][n];
        int v[n];
        for(int k=0;k<n;k++)
            v[k]=k;
        for(int p=0;p<n;p++){
            for(int q=0;q<n;q++)
                num[p][q]=1;
        }
        for(int j=0;j<20;j++){
            int sum[n];
            for(int k=0;k<n;k++)
                sum[k]=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++)
                    sum[i]+=num[j][i]*v[j];
            }
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(10*n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
    for(int i=1;i<8;i++){//n=100-700
        time=0;
        const int n=100*i;
        int num[n][n];
        int v[n];
        for(int k=0;k<n;k++)
            v[k]=k;
        for(int p=0;p<n;p++){
            for(int q=0;q<n;q++)
                num[p][q]=1;
        }
        for(int j=0;j<20;j++){
            int sum[n];
            for(int k=0;k<n;k++)
                sum[k]=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++)
                    sum[i]+=num[j][i]*v[j];
            }
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(100*n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

void cache(){
    cout<<"This is cache algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for(int i=1;i<10;i++){//n=10-90
        time=0;
        const int n=10*i;
        int num[n][n];
        int v[n];
        for(int k=0;k<n;k++)
            v[k]=k;
        for(int p=0;p<n;p++){
            for(int q=0;q<n;q++)
                num[p][q]=1;
        }
        for(int j=0;j<20;j++){
            int sum[n];
            for(int k=0;k<n;k++)
                sum[k]=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for(int j=0;j<n;j++){
                for(int i=0;i<n;i++)
                    sum[i]+=num[j][i]*v[j];
            }
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(10*n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
    for(int i=1;i<8;i++){//n=100-700
        time=0;
        const int n=100*i;
        int num[n][n];
        int v[n];
        for(int k=0;k<n;k++)
            v[k]=k;
        for(int p=0;p<n;p++){
            for(int q=0;q<n;q++)
                num[p][q]=1;
        }
        for(int j=0;j<20;j++){
            int sum[n];
            for(int k=0;k<n;k++)
                sum[k]=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for(int j=0;j<n;j++){
                for(int i=0;i<n;i++)
                    sum[i]+=num[j][i]*v[j];
            }
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(100*n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

int main()
{
    common();
    cout<<endl;
    cache();
    return 0;
}
