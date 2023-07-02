#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
#include<mpi.h>
using namespace std;
#define N 130
#define ELI 8

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

    int step = 5;
    struct timeval start,end;

    int rank; //进程数
    int size; //当前是第几个进程
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    //设置进程分配范围[r1,r2]
    int tasknum=ELI/size;
    int r1 = rank*tasknum;
    int r2;
    if(rank!=size-1)
        r2 = r1 + tasknum -1;
    else r2 = ELI-1;

    gettimeofday(&start,NULL);
    for(int i = N-1; i>=0; i-=step)
    {
        if(i-step < 0)
            step = i+1;
        for(int j=r1; j<=r2; j++)
        {
            if(!E[j].upgrade)
            {
                for(int k = 0; k<step; k++)
                {
                    if(E[j].bit[i-k]== 1) 
                        E[j].bit^=R[i-k]; 
                    if(E[j].bit.none())
                    {
                        E[j].upgrade=true;
                        break;
                    }
                    else
                    {
                        for(int l = E[j].first;l>=0;l--){
                            if(E[j].bit[l] == 1)
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
        //0号进程接收被消元行数据
        if(rank!=0)
	        MPI_Send(&E[r1],(r2-r1+1)*sizeof(line),MPI_BYTE,0,1,MPI_COMM_WORLD);
        else{
            for(int q=1; q<size; q++){
	        if(q!=size-1)
		        MPI_Recv(&E[q*tasknum],tasknum*sizeof(line),MPI_BYTE,q,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	        else MPI_Recv(&E[q*tasknum],sizeof(line)*(tasknum+ELI%size),MPI_BYTE,q,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
        //0号进程执行升格和后续处理
        if(rank==0)
        {
            for(int j=r1;j<ELI;j++)
            {
                if(E[j].upgrade)
	            {
                    if(R[E[j].first].none())
                        R[E[j].first]^=E[j].bit;
		            else
		            {
                        E[j].upgrade=false;
		                for(int k = 0; k<step; k++)
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
                                for(int l = E[j].first;l>=0;l--)
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
        //升格处理完成，将数据发送给其他进程
        if(rank==0){
	        for(int q=1;q<size;q++)
            {
                if(q!=size-1)
                    MPI_Send(&E[q*tasknum],tasknum*sizeof(line),MPI_BYTE,q,2,MPI_COMM_WORLD);
                else MPI_Send(&E[q*tasknum],sizeof(line)*(tasknum+ELI%size),MPI_BYTE,q,2,MPI_COMM_WORLD);
            }
        }
	    if(rank!=0)
	        MPI_Recv(&E[r1],(r2-r1+1)*sizeof(line),MPI_BYTE,0,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    MPI_Finalize();

    gettimeofday(&end,NULL);
    float time = ((end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec);
    cout<<time<<endl;

    return 0;
}
