#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#define checkCudaErrors(ans) { gpuAssert((ans), __FILE__, __LINE__); }
template<class T>
struct MySM
{
    __device__ inline operator       T *()
    {
        extern __shared__ int __sharememory[];
        return (T *)__sharememory;
    }

    __device__ inline operator const T *() const
    {
        extern __shared__ int __sharememory[];
        return (T *)__sharememory;
    }
};
inline void gpuAssert(cudaError_t mc, const char *mf, int ml, bool abort=true)
{
   if (mc != cudaSuccess) 
   {
      if (abort) exit(mc);
   }
}