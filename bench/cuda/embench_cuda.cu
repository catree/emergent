/* Example of integrating CUDA functions into an existing 
 * application / framework.
 * Host part of the device code.
 * Compiled with Cuda compiler.
 */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "embench_cuda.h"

/*
Description of Algorithm

This is a receiver-based algorithm.
Each thread computes partial d_nets inputs for a single Unit. 
Each such partial computation is called a "chunk" -- there
are 32 weights per chunk; therefore, each projection is
represented in atoms of chunks; any unused are set to 0:0.f
(i.e. Unit 0 is also a dummy.)

The Warp size (N_THREADS) is used to make sure that different
units are processed in parallel-- the allocation algorithm 
insures that each warp uses a different d_nets unit, so the write
updates do not need to be syncronized. 
*/

// Types

typedef struct con {
  int	snd_idx;
  float	wt;
} con_t;


typedef con_t con_chunk_t[CON_CHUNK_SZ];
typedef int blk_map_t[N_THREADS];
typedef con_chunk_t con_blk_t[N_THREADS];

// Global data structures

//__device__
uint	h_n_units;
//__device__
uint	n_con_chunks; // just for reference
//__device__
uint	n_blks; // number of [N_THREADS] blocks needed

__constant__
float		c_acts[MAX_UNITS]; // only [h_n_units] init'ed/used
float*		d_nets; // [h_n_units];
blk_map_t*	d_blk_map; // [n_blks][N_THREADS]
con_blk_t*	d_con_blks; // [n_blks][N_THREADS]


__global__ 
void kRecv_Netin(
  float* d_nets,
  blk_map_t* d_blk_map,
  con_blk_t* d_con_blks
) {
  // block/thread indexes, for clarity
  int blk = blockIdx.x;
  int tx = threadIdx.x;
  int un_idx = d_blk_map[blk][tx];
  con_chunk_t& con_chunk = d_con_blks[blk][tx];
  float tnet = 0.0f;
  for (int ci = 0; ci < CON_CHUNK_SZ; ci++) {
    tnet += c_acts[con_chunk[ci].snd_idx] * con_chunk[ci].wt; 
  }
  d_nets[un_idx] += tnet; 
//d_nets[tx] = 1.0f / (tx + 1);
}

extern "C" {

int cuAllocMem(uint n_units_, uint silo_sz_) {
  if ((n_units_ == 0) || (silo_sz_ == 0))
    return 1;
    
  // round up number of blks needed to even processable number
  
   // round up the number of chunks needed to an even processable number
  n_con_chunks = (silo_sz_ * N_THREADS); // for reference
printf("cuAllocMem: n_con_chunks=%d\n",n_con_chunks); 
  if ((n_units_ > MAX_UNITS) || (silo_sz_ > MAX_SILO_SZ))
    return 2;
    
  h_n_units = n_units_;
  n_blks = silo_sz_; // s/b min 1
  
  CUT_DEVICE_INIT();
  
  uint mem_size;
  // init used acts to 0 
  mem_size = n_units_ * sizeof(float);
  void* mem = calloc(n_units_, sizeof(float));
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("c_acts", mem, mem_size));
  free(mem);
  // alloc nets (no init, since overwritten anyway)
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_nets, mem_size));
  
  
//printf("cuAllocMem: n_blks=%d\n",n_blks); 
  // block map
  mem_size = n_blks * sizeof(blk_map_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_blk_map, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_blk_map, 0, mem_size));
    
  mem_size = n_blks * sizeof(con_blk_t);
  CUDA_SAFE_CALL(cudaMalloc((void**) &d_con_blks, mem_size));
  CUDA_SAFE_CALL(cudaMemset(d_con_blks, 0, mem_size));
  
  //TODO: error?
  return 0;
}

int cuFreeMem() {
  CUDA_SAFE_CALL(cudaFree((void*) d_nets));
  d_nets = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_blk_map));
  d_blk_map = NULL;
  CUDA_SAFE_CALL(cudaFree((void*) d_con_blks));
  d_con_blks = NULL;

  return 0;
}

int cuCpHD_Acts(const float* acts) {
  CUDA_SAFE_CALL(cudaMemcpyToSymbol("c_acts", acts, h_n_units * sizeof(float)));
  return 0;
}

int cuCpHD_Cons(cbGetCon GetCon) {
  int chunks_copied = 0; // sanity check
  con_chunk_t con_buf; // NOTE: maybe too big to safely alloc on stack...
// we work one silo at a time, each silo is a warp thread
  for (int silo = 0; silo < N_THREADS; silo++) {
    int un_idx = silo;
    int blk = 0; // block x
    while (un_idx < h_n_units) {
      int con_idx = 0;
      bool done = false;
      while (!done) {
        int chunk_idx = 0; // note: only advanced if val read, so ==0 means none read
        while (chunk_idx < CON_CHUNK_SZ) {
          con_t& con = con_buf[chunk_idx];
          done = GetCon(un_idx, con_idx, &(con.snd_idx), &(con.wt));
          if (done) break;
          con_idx++;
          chunk_idx++;
        }
        // chunk_idx is now # of cons read
        if (chunk_idx > 0) { // write chunk and blk map entry
          void* ptr;
          int flat_chunk_idx = (blk * N_THREADS) + silo;
          // block map guy -- points to this unit
          ptr = (void*)((char*)d_blk_map + (flat_chunk_idx * sizeof(int)));
          CUDA_SAFE_CALL(cudaMemcpy(ptr, &un_idx,
            sizeof(int), cudaMemcpyHostToDevice));
          
          // chunk guy -- only write used portion, so leftover stays 0 in device
          ptr = (void*)((char*)d_con_blks + (flat_chunk_idx * sizeof(con_chunk_t)));
          CUDA_SAFE_CALL(cudaMemcpy(ptr, &con_buf,
            sizeof(con_t) * chunk_idx, cudaMemcpyHostToDevice));
            
          // bump block pointers/counters
          chunks_copied++;
          blk++;
        }
      }
      un_idx += N_THREADS;
    }
  }
printf("Copied %d chunks (%d expected)\n", chunks_copied, n_con_chunks);  
    
  return 0;
}

int cuCpDH_Nets(float* nets) {
  CUDA_SAFE_CALL(cudaMemcpy((void*)nets, (void*)d_nets,
    h_n_units * sizeof(float), cudaMemcpyDeviceToHost));
  return 0;
}

void cuRecv_Netin()
{

    // setup execution parameters
    dim3 grid(n_blks, 1, 1);
    dim3 threads(N_THREADS, 1, 1);
    
    // clear nets
    CUDA_SAFE_CALL(cudaMemset(d_nets, 0, h_n_units * sizeof(float)));
    
    // execute the kernel
    kRecv_Netin<<< grid, threads >>>(d_nets, d_blk_map, d_con_blks);

    // check if kernel execution generated and error
    CUT_CHECK_ERROR("Kernel execution failed");
}


};
