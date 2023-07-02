#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
#include<omp.h>
using namespace std;
#define N 130
#define ELI 8
#define num_threads 4
#define chunksize 1

struct line{
    bool upgrade;  //是否升级为消元子
    int first;  //首项
    bitset<N> bit;
};

bitset<N> R[N]; //消元子 R
line E[ELI]; //被消元行 E

int main()
{
    ifstream file;
    string num;

    //读消元子
    file.open("/home/ss2110577/Groebner/1_130_22_8/1.txt",ios_base::in); 
    while(getline(file,num))
    {
        bool flag = true;
        int i; //当前读入第i行
        stringstream st;
        int tmp; //每次读入一个数
        st<<num;
        while(st>>tmp)
        {
            if(flag) //设置首项i
            {
                i = tmp;
                flag = false;
            }
            R[i].set(tmp);
        }      
    }
    file.close();
    file.clear(ios::goodbit);

    //读被消元行
    file.open("/home/ss2110577/Groebner/1_130_22_8/2.txt",ios_base::in);
    int p = 0;
    while(getline(file,num))
    {
        bool flag = true;
        stringstream st;
        int tmp;
        st<<num;
        E[p].upgrade=false;
        while(st>>tmp)
        {
            if(flag) //设置首项
            {
                E[p].first=tmp;
                flag = false;
            }
            E[p].bit.set(tmp);
        }  
        p++;    
    }
    file.close();

    int step = 5; //步长

    struct timeval start,end;
    gettimeofday(&start,NULL);

    int i, j, k, l;
//外循环之外创建线程，避免线程反复创建销毁
#pragma omp parallel if(1), num_threads(num_threads),private(i, j, k, l)
    for(i = N-1;i>=0;i-=step)
    {
        if(i-step<0)
            step=i+1;
#pragma omp for schedule(static,chunksize) //并行部分
        for(j = 0; j<ELI; j++)
        {
            if(!E[j].upgrade)
            {
                for(k = 0; k<step; k++)
                {
                    if(E[j].bit[i-k]==1)
                        E[j].bit^=R[i-k];
                    if(E[j].bit.none())
                    {
                        E[j].upgrade=true;
			            break;
                    }
                    else
                    {
                        for(l = E[j].first;l>=0;l--)
                        {
                            if(E[j].bit[l]==1)
                            {
                                E[j].first = l;
				                break;
                            }
                        }
                        if(R[E[j].first].none())
                        {
                            E[j].upgrade=true;
			                break;
                        }
                    }                
                }
                
            }       
	    }

#pragma omp single //串行部分
	    for(j=0;j<ELI;j++)
        {
            if(E[j].upgrade)
            {
                if(R[E[j].first].none())
                     R[E[j].first]^=E[j].bit;
                else
                {
                    E[j].upgrade=false;
                    for(k = 0; k<step; k++)
                    {
                        if(E[j].bit[i-k]==1)
                            E[j].bit^=R[i-k];
                        if(E[j].bit.none())
                        {
                            E[j].upgrade=true;
                            break;
                        }
                        else
                        {
                            for(l = E[j].first;l>=0;l--)
                            {
                                if(E[j].bit[l]==1)
                                {
                                    E[j].first = l;
                                    break;
                                }
                            }
                            if(R[E[j].first].none())
                            {
                                R[E[j].first]^=E[j].bit;
                                E[j].upgrade=true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } 
    gettimeofday(&end,NULL);

    float time = ((end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec);
    cout<<time<<endl;

    return 0;
}
