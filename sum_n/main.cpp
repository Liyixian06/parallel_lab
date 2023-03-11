#include <iostream>
#include <math.h>
#include <windows.h>

using namespace std;

void common(){
    cout<<"This is common algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for(int i=1;i<19;i++){
        time=0;
        const int n=pow(2,i);
        int num[n];
        for (int k=0;k<n;k++)
            num[k]=1;
        for(int j=0;j<20;j++){
            int sum=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for (int l=0;l<n;l++)
                sum+=num[l];
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(2^n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

void multi_2(){
    cout<<"This is multi(2)-way algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for (int i=1;i<19;i++){
        time=0;
        const int n=pow(2,i);
        int num[n];
        for (int k=0;k<n;k++)
            num[k]=1;
        for(int j=0;j<20;j++){
            int sum1=0,sum2=0,sum=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for (int l=0;l<n;l+=2){
                sum1+=num[l];
                sum2+=num[l+1];
            }
            sum=sum1+sum2;
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(2^n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

void multi_4(){
    cout<<"This is multi(4)-way algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for (int i=2;i<19;i+=2){
        time=0;
        const int n=pow(2,i);
        int num[n];
        for (int k=0;k<n;k++)
            num[k]=1;
        for(int j=0;j<20;j++){
            int sum1=0,sum2=0,sum3=0,sum4=0,sum=0;
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for (int l=0;l<n;l+=4){
                sum1+=num[l];
                sum2+=num[l+1];
                sum3+=num[l+2];
                sum4+=num[l+3];

            }
            sum=sum1+sum2+sum3+sum4;
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(2^n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

void recur(){
    cout<<"This is recursion algorithm."<<endl;
    long long head, tail, freq;
    float time;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    for (int i=1;i<19;i++){
        time=0;
        const int n=pow(2,i);
        int num[n];
        for (int k=0;k<n;k++)
            num[k]=1;
        for(int j=0;j<20;j++){
            //start time
            QueryPerformanceCounter((LARGE_INTEGER *)&head);
            for (int m=n;m>1;m/=2){
                for(int p=0;p<m/2;p++)
                    num[p]=num[p*2]+num[p*2+1];
            }
            //end time
            QueryPerformanceCounter((LARGE_INTEGER *)&tail);
            time+=(tail-head)*1000000.0/freq;
        }
        cout<<"n(2^n)="<<i<<",";
        cout<<"Time:"<<time/20.0<<"us"<<endl;
    }
}

int main()
{
    common();
    cout<<endl;
    multi_2();
    cout<<endl;
    multi_4();
    cout<<endl;
    recur();
    return 0;
}
