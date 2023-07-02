#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<stdio.h>
#include<bits/stdc++.h>
#include<vector>
#include<sys/time.h>
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

    int step = 5; //步长

    struct timeval start,end;
    gettimeofday(&start,NULL);

    for(int i = N-1; i>=0; i-=step) //每轮导入一个step的消元子
    {
        if(i-step < 0)
            step = i+1;
        for(int j = 0; j<ELI; j++) //遍历被消元行
        {
            if(!E[j].upgrade) // 若被消元行已经升格则跳过
            {
                for(int k = 0; k<step; k++) //每次看一行消元子
                {
                    if(E[j].bit[i-k]== 1) //如果被消元行有对应首项，就消元
                        E[j].bit^=R[i-k]; 
                    if(E[j].bit.none()) //检查是否被消为全0，如果是则标记已升格
                    {
                        E[j].upgrade=true;
                        break;
                    }
                    else //如果被消元行没有对应首项
                    {
                        for(int l = E[j].first;l>=0;l--) //更新被消元行的首项
                        {
                            if(E[j].bit[l] == 1)
                            {
                                E[j].first = l;
                                break;
                            }
                        }
                        if(R[E[j].first].none()) //若被消元行首项对应的消元子不存在，将其升格为消元子
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
    gettimeofday(&end,NULL);

    float time = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
    cout<<time<<endl;
    
    return 0;
}
