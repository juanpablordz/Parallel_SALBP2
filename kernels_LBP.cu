//System Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <vector>

//CUDA runtime 
#include <cuda.h>
#include <cuda_runtime.h>

// Helper functions to work with CUDA
#include <helper_cuda.h>
#include <helper_functions.h>
#include "device_functions.h"

//Proyect files includes
#include "BinaryRestriction.h"
#include "VectorBR.h"
#include "BinaryMixer.h"

//Define Block SIZE.
int block_width = 1024;

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort = true)
{
	if (code != cudaSuccess)
	{
		fprintf(stderr, "GPUassert: \"%s\": %s %s %d\n", cudaGetErrorName(code), cudaGetErrorString(code), file, line);
		if (abort) exit(code);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////PARALLEL MIX Function/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


__global__ void ParallelMix(unsigned long long int * d_cont ,unsigned int number_threads, int d_rest_size ,int numStations, unsigned int * d_temp, unsigned int * d_rest, unsigned int * d_out) {

	//Store thread ID
	const unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;

	//Condition that verifies the usefulness of the thread.
	if (tid < number_threads) {

		//VARIABLES

		//This auxiliar variable signals when there has been a double activity assignment. 
		//IF (aux_v > 0) then a double assignment has been done. 
		unsigned int aux_v = 0;

		//This auxiliar variable stores information about which activities have been asigned.
		unsigned int cum_v = 0;

		//The purpose of flag is to identify when a double activity assignment has been made.
		bool flag = false;

		//This auxiliar vector has to be modified according to the number of stations in a given problem.
		unsigned int aux[7] = { 0,0,0,0,0,0,0 };

		//PRINTS
		//printf("Thread Num: %d \n", number_threads);
		//printf("Thread ID: %d \n", tid);

		//FOR-LOOP: Iterates over all solution tables from Constraint (Ri). 
		//Each thread (tid) is going to intersect every solution table in the new constraint (d_temp).
		for (int i = 0; i <d_rest_size ; i++) {

			//Perform OR operations on every line (STATION) of the solution table.

			//The objective is to identify if a single activity is  assigned to more than 1 station.
			for (int k = 0; k<numStations; k++) {

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				////////////////////////////////////////////////BOOLEAN INTERSECTION ALGORITHM////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//1. Perform OR operation on table row (k) from tid solution table (d_temp) and new constraint (d_rest).
				//	 Store result in aux[k].cx
				//		1.1. Perform AND operation with aux[k] and cum_v. 
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				aux[k] = *(d_temp + (tid*numStations + k)) | *(d_rest + k + (i*numStations));

				if (!flag) {
					aux_v = cum_v & aux[k];
					if (aux_v > 0) {
						flag = true;
					}
					cum_v = cum_v^aux[k];
				}
			}

			if (!flag) {
				//Tabla VALIDA
				aux_v = atomicAdd(d_cont, 1);

				for (int j = 0; j < numStations; j++) {
					*(d_out + (aux_v*numStations) + j) = aux[j];
					//printf("contador: %d\n", *d_cont);
				}
				//printf("VALIDA %d, %d\n", tid, i);
				flag = false;
				aux_v = 0;
				cum_v = 0;

			}
			else {
				//Tabla INVALIDA
				//printf("INVALIDA %d, %d\n", tid, i);
				flag = false;
				aux_v = 0;
				cum_v = 0;

			}
		}
		//Termina forloop
		//free memory from the variables

	}
}

extern "C" bool
runTest(int argc, const char **argv, unsigned long long int * h_cont, int numStations, unsigned int * h_temp, unsigned long long int h_temp_size , unsigned int * h_rest, int h_rest_size, unsigned int * h_out) {

	//use command-line specified CUDA device, otherwise use device with highest Gflops
	//es necesario?
	findCudaDevice(argc, (const char **)argv);

	//Number of threads and blocks
	unsigned int num_threads = h_temp_size / numStations;
	unsigned int num_blocks = 1 + (num_threads / block_width);
	//Prints
	cout << "Number of threads: " << num_threads << endl;
	cout << "Number of Blocks:  " << num_blocks << endl;
	printf("Out vector size: %s\n", to_string((h_temp_size/numStations) * (h_rest_size / numStations)));


	//GPU memory status variables
	size_t total_mem, free_mem;

	//Memory requirement for restriction
	const size_t mem_size_rest = sizeof(int) * h_rest_size;

	//Create Device Variables
	unsigned int * d_rest;
	unsigned int * d_out;
	unsigned long long int * d_cont;
	unsigned long long int * internal_cont = new unsigned long long int[1];
	*(internal_cont) = 0;


	//Allocate device memory CONT y REST
	checkCudaErrors(cudaMalloc((void **)&d_rest, mem_size_rest));
	checkCudaErrors(cudaMalloc((void **)&d_cont, sizeof(unsigned long long int)));

	//Copy memory from host to device CONT y REST
	checkCudaErrors(cudaMemcpy(d_rest, h_rest, mem_size_rest, cudaMemcpyHostToDevice));
	//realmente es necesario?
	gpuErrchk(cudaMemcpy(d_cont, h_cont, sizeof(unsigned long long int), cudaMemcpyHostToDevice));

	//GPU memory status REPORT.
	cudaMemGetInfo(&free_mem, &total_mem);
	printf("Free Memory: %s \tTotal Memory: %s \n", to_string(free_mem), to_string(total_mem));


	//PRINT: Memory usage by vectors.
	/*
	cout << "Out vector byte size: " << to_string(mem_size_out) << endl;
	cout << "Rest vector size: " << h_rest_size << endl;
	
	*/

	//Declare a 1 MB constant
	const size_t MB = 1 << 20;

	//Buffer Conffiguration: It will manage the batches to be processed in the GPU
	cudaError_t status;
	unsigned int * buffer;

	//Allocate at most 500MB of memory for the buffer size.
	//This limitation will allow us to allocate enough memory to store the resulting solutions vector.
	size_t buffer_size = std::min(4294967296 / (h_rest_size / numStations), (long long)536870912);
	//Make sure the allocation is succesful
	for (; buffer_size > MB; buffer_size -= MB) {
		//Allocate memory for buffer
		status = cudaMalloc((void **)&buffer, buffer_size);
		if (status == cudaSuccess)
			break;
	}

	//PRINT: Number of bytes allocated for GPU.
	cout << "Allocated " << buffer_size << " bytes on GPU. " << endl;

	//PRINT: Number of elements that must be processed.
	cout << h_temp_size << " int items require processing. " << endl;


	// Helps preserve the integrity of the solution vector
	// which has the size of the number of stations.
	size_t aux_buff_size = (buffer_size / sizeof(int)) % numStations;

	// Number of int elements that will be proccessed in this batch.
	// We substract the aux_buff_size in order to
	// garanty the integrity of the solution vectors.
	size_t batchN = (buffer_size - aux_buff_size) / sizeof(int);

	//The number fof elements that await processing.
	size_t remainN = h_temp_size;

	// Creation of output vector at most size Restriction.size() * Temporal.size()
	int elements_out = batchN * (h_rest_size / numStations);
	const size_t mem_size_out = elements_out * sizeof(int);

	//Allocate memory on GPU device
	checkCudaErrors(cudaMalloc((void **)&d_out, mem_size_out));

	//PRINT: Batch size.
	cout << "Number of int proccessed in this batch: " << batchN << endl;

	//LOOP through host source data in batches.
	for (; remainN > 0; remainN -= batchN) {

		cout << "\n BATCH \n\n";

		//Si remain es menor que el tamano del batch,
		//ajusto el tamano del batch a lo que queda de datos
		//y en caso contrario lo dejo igual
		batchN = (remainN<batchN) ? remainN : batchN;
		size_t worksize = batchN * sizeof(int);
		cout << "Processing " << batchN << "/" << remainN << " items remaining." << endl;

		//COPY values from host temporal solution to buffer.
		//Process the items in buffer.
		cudaMemcpy(buffer, h_temp, worksize, cudaMemcpyHostToDevice);

		//The number of threads is determined by the number of Solution tables in the vector.
		num_threads = batchN / numStations;
		num_blocks = 1 + (num_threads / block_width);

		//Print: Number of Threads and Blocks to be used in this batch.
		cout << "Number of threads: " << num_threads << endl;
		cout << "Number of Blocks:  " << num_blocks << endl;

		/////////////////
		//Launch kernel//
		/////////////////
		ParallelMix << < num_blocks, block_width >> >(d_cont, num_threads, h_rest_size / numStations, numStations, buffer, d_rest, d_out);

		//Print: Report succesful execution.
		cout << "Succesful execution\n";

		//Copy BATCH counter from GPU to CPU. 
		gpuErrchk(cudaMemcpy(h_cont, d_cont, sizeof(unsigned long long int), cudaMemcpyDeviceToHost));

		//Acumulate value of BATCH COUNTERS IN A GLOBAL COUNTER.
		//BATCH COUNTER: h_cont and d_cont.
		//GLOBAL COUNTER: internal_cont
		*(internal_cont) += *(h_cont);

		//PRINT: Counters
		cout << *(h_cont) << endl;
		cout << "ACUM: " << *(internal_cont) << endl;

		//COPY valid solutions from device to host vector.
		gpuErrchk(cudaMemcpy(h_out, d_out,*(h_cont)*numStations*sizeof(int), cudaMemcpyDeviceToHost));
		
		///////////////////
		//UPDATE pointers//
		///////////////////

		//Increase Temp pointer in the magnitude of the proccessed batch.
		h_temp += batchN;
		//Increase the h_out pointer in the magnitude of the number of valid solutions from the proccessed batch.
		h_out = h_out + (*(h_cont)*numStations);

		//Reset. Value of out Batch counter and d_out vector space to be used.
		gpuErrchk(cudaMemset(d_cont, (unsigned long long int)0, sizeof(unsigned long long int)));
		gpuErrchk(cudaMemset(d_out, 0,mem_size_out));
	}

	//GPU memory status REPORT.
	cudaMemGetInfo(&free_mem, &total_mem);
	printf("Free Memory: %s \tTotal Memory: %s \n", to_string(free_mem), to_string(total_mem));

	//Copy counter variable back to the host.
	*(h_cont) = *(internal_cont);

	//cleanup device memory
	//Reset Device
	cudaDeviceReset();

	return 0;

}

////cleanup device memory
////checkCudaErrors(cudaFree(d_out));
////checkCudaErrors(cudaFree(d_temp));
////checkCudaErrors(cudaFree(d_rest));
////checkCudaErrors(cudaFree(buffer));

////Reset Device
//cudaDeviceReset();

