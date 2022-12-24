
# include "mygm.h"

__global__ void
myreduce_data(double *gid, double *god, unsigned int sizen)
{
    double *mysd = MySM<double>();
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*blockDim.x + threadIdx.x;
    mysd[tid] = (i < sizen) ? gid[i] : 0;
    __syncthreads();
    for (unsigned int s=blockDim.x/2; s>0; s>>=1)
    {
        if (tid < s)
        {
            if(mysd[tid] < mysd[tid + s])
                mysd[tid] = mysd[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0) god[blockIdx.x] = mysd[0];
}
extern "C" double mygetmax(int n, int mean, int seed)
{
    int size = 1<<n; 
    int maxThreads = 256; 
    double maxvalue;
    unsigned int bytes = size * sizeof(double);
    double *h_idata = (double *) malloc(bytes);
    srand48(seed);
    for (int i=0; i<size; i++)
    {
        h_idata[i] = -mean * log(drand48());
    }
    int numBlocks = size / maxThreads; 
    int numThreads = size;
    int smemSize = maxThreads * sizeof(double);
    double *h_odata = (double *) malloc(numBlocks*sizeof(double));
    double  *d_idata = NULL;
    double *d_odata = NULL;
    double *d_intermediate = NULL;
    checkCudaErrors(cudaMalloc((void **) &d_idata, bytes));
    checkCudaErrors(cudaMalloc((void **) &d_odata, numBlocks*sizeof(double)));
    checkCudaErrors(cudaMalloc((void **) &d_intermediate, numBlocks*sizeof(double)));
    checkCudaErrors(cudaMemcpy(d_idata, h_idata, bytes, cudaMemcpyHostToDevice));
    myreduce_data<<<numBlocks,maxThreads,smemSize>>>(d_idata, d_odata, numThreads);
    int s = numBlocks;
    while (s > 1) {
    checkCudaErrors(cudaMemcpy(d_intermediate, d_odata, s * sizeof(double),
                                cudaMemcpyDeviceToDevice));
        myreduce_data<<<(s+maxThreads-1)/maxThreads,maxThreads,smemSize>>>(d_intermediate, d_odata, s);
        s = (s+maxThreads-1)/maxThreads;
    }
    checkCudaErrors(cudaMemcpy(h_odata, d_odata, sizeof(double), cudaMemcpyDeviceToHost));
    maxvalue = h_odata[0];
    checkCudaErrors(cudaFree(d_idata));
    checkCudaErrors(cudaFree(d_odata));
    return maxvalue;
}
