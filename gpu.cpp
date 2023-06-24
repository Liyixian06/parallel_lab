#include <CL/sycl.hpp>
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace sycl;

#define N 1024
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
    return;
}

float cpu(float m[N][N])
{
    float(*new_m)[N] = (float(*)[N])malloc(N * N * sizeof(float));
    memcpy(new_m, m, sizeof(float) * N * N);

    struct timeval start;
    struct timeval end;//clock
    gettimeofday(&start,NULL);

    for(int k=0; k<N; k++){
        for(int j=k+1; j<N; j++)
            new_m[k][j] = new_m[k][j] / new_m[k][k];
        new_m[k][k] = 1.0;
        for(int i=k+1; i<N; i++){
            for(int j=k+1; j<N; j++)
                new_m[i][j] -= new_m[i][k]*new_m[k][j];
            new_m[i][k] = 0;
        }
    }
    
    gettimeofday(&end,NULL);
    float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
    
    return time;
}

float gpu(float m[N][N])
{
    auto propList = cl::sycl::property_list {cl::sycl::property::queue::enable_profiling()};
    queue my_gpu_queue( cl::sycl::gpu_selector_v , propList); 
    cout << "Device: " << my_gpu_queue.get_device().get_info<info::device::name>() << "\n";     

    //共享内存
    float(*new_m)[N] = (float(*)[N])malloc_shared<float>(N*N, my_gpu_queue);
    memcpy(new_m, m, sizeof(float)*N*N);
    
    struct timeval start;
    struct timeval end;//clock
    gettimeofday(&start,NULL);
    
    for (int k=0; k<N; k++){
        my_gpu_queue.parallel_for(range{(unsigned long)(N-(k+1))}, [=](item<1> idx)
        {
            int j = idx[0]+k+1; // for(int j=k+1; j<N; j++)
            new_m[k][j] = new_m[k][j] / new_m[k][k];
        }).wait();
        new_m[k][k]=1.0;
        my_gpu_queue.parallel_for(range{(unsigned long)(N-(k+1))}, [=](item<1> idx)
        {
            int i = idx[0]+k+1; // for(int i=k+1; i<N; i++)
            for (int j=k+1; j<N; j++)
                new_m[i][j] -= new_m[i][k] * new_m[k][j]; 
            new_m[i][k]=0;
        }).wait();
    }

    gettimeofday(&end,NULL);
    float time=(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec;
    
    return time;
}

int main()
{
    m_reset();
    cout << "矩阵大小：" << N << " * " << N <<"\n";
    cout << "串行算法用时: " << cpu(m) <<"\n";
    cout << "并行算法用时：" << gpu(m) <<"\n";

    return 0;
}
