
/*
 To compile: 
    nvcc -arch=sm_60 reduction_kernel.cu
 To run with the array size 2^20, expo dist mean 5, and init seed 17:
    ./a.out 20 5 17
 */
#include <stdio.h>
#include <stdlib.h>

template<class T>
struct SharedMemory
{
    __device__ inline operator       T *()
    {
        extern __shared__ int __smem[];
        return (T *)__smem;
    }

    __device__ inline operator const T *() const
    {
        extern __shared__ int __smem[];
        return (T *)__smem;
    }
};

/*
    Parallel sum reduction using shared memory
    - takes log(n) steps for n input elements
    - uses n threads
    - only works for power-of-2 arrays
*/

/*
    This version uses sequential addressing -- no divergence or bank conflicts.
*/
__global__ void
reduce(double *g_idata, double *g_odata, unsigned int n)
{
    double *sdata = SharedMemory<double>();

    // load shared mem
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? g_idata[i] : 0;

    __syncthreads();

    // do reduction in shared mem
    for (unsigned int s=blockDim.x/2; s>0; s>>=1)
    {
        if (tid < s)
        {
            if(sdata[tid] < sdata[tid + s])
                sdata[tid] = sdata[tid + s];
        }

        __syncthreads();
    }

    // write result for this block to global mem
    if (tid == 0) g_odata[blockIdx.x] = sdata[0];
}


// CUDA Runtime
#include <cuda_runtime.h>

#define checkCudaErrors(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

extern "C" double get_max(int n, int mean, int seed)
{
    int size = 1<<n;    // number of elements (at least 256) to reduce
    int maxThreads = 256;  // number of threads per block
    double maxvalue;

    // create random input data on CPU
    unsigned int bytes = size * sizeof(double);

    double *h_idata = (double *) malloc(bytes);

    srand48(seed);
    for (int i=0; i<size; i++)
    {
        // h_idata[i] = 1.0; // for testing
        // expo dist with mean 
        h_idata[i] = -mean * log(drand48());
    }

    int numBlocks = size / maxThreads; 
    int numThreads = size;

    int smemSize = maxThreads * sizeof(double);

    // allocate mem for the result on host side
    double *h_odata = (double *) malloc(numBlocks*sizeof(double));

    // allocate device memory and data
    double  *d_idata = NULL;
    double *d_odata = NULL;
    double *d_intermediate = NULL;

    checkCudaErrors(cudaMalloc((void **) &d_idata, bytes));
    checkCudaErrors(cudaMalloc((void **) &d_odata, numBlocks*sizeof(double)));
    checkCudaErrors(cudaMalloc((void **) &d_intermediate, numBlocks*sizeof(double)));

    // copy data directly to device memory
    checkCudaErrors(cudaMemcpy(d_idata, h_idata, bytes, cudaMemcpyHostToDevice));

    reduce<<<numBlocks,maxThreads,smemSize>>>(d_idata, d_odata, numThreads);

    int s = numBlocks;

    while (s > 1) {
    checkCudaErrors(cudaMemcpy(d_intermediate, d_odata, s * sizeof(double),
                                cudaMemcpyDeviceToDevice));
        reduce<<<(s+maxThreads-1)/maxThreads,maxThreads,smemSize>>>(d_intermediate, d_odata, s);
        s = (s+maxThreads-1)/maxThreads;
    }

    checkCudaErrors(cudaMemcpy(h_odata, d_odata, sizeof(double), cudaMemcpyDeviceToHost));

    maxvalue = h_odata[0];

    checkCudaErrors(cudaFree(d_idata));
    checkCudaErrors(cudaFree(d_odata));

    return maxvalue;
}
