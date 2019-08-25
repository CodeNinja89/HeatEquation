#include "kernel.h"

#define TX 32 // number of threads along x-axis
#define TY 32 // number of threads along y-axis
#define RAD 1

int divUp(int a, int b) {
	return (a + b - 1) / b; // number of blocks of a specified size to cover a computational grid
}

__device__ int idxClip(int idx, int idxMax) {
	return idx > (idxMax - 1) ? (idxMax - 1) : (idx < 0 ? 0 : idx); // make sure indexes are between (0, N - 1)
}

__device__ int flatten(int col, int row, int width, int height) {
	return idxClip(col, width) + idxClip(row, height) * width; // flatten a 2D array to 1D	
}

__device__ unsigned char clip(int n) {
	return n > 255 ? 255 : (n < 0 ? 0 : n);
}

__global__ void resetKernel(float *d_temp, int w, int h, BC bc) {
	const int col = blockIdx.x * blockDim.x + threadIdx.x;
	const int row = blockIdx.y * blockDim.y + threadIdx.y;

	if((col >= w) || (row >= h)) return;
	d_temp[row * w + col] = bc.t_a;
}

__global__ void tempKernel(uchar4 *d_out, float *d_temp, int w, int h, BC bc) {
	extern __shared__ float s_in[];

	const int col = blockIdx.x * blockDim.x + threadIdx.x;
	const int row = blockIdx.y * blockDim.y + threadIdx.y;
	if((col >= w) || (row >= h)) return;

	const int idx = flatten(col, row, w, h);

	// local height and width
	const int s_w = blockDim.x + 2 * RAD;
	const int s_h = blockDim.y + 2 * RAD;

	// shared memory indices
	const int s_col = threadIdx.x + RAD;
	const int s_row = threadIdx.y + RAD;
	const int s_idx = flatten(s_col, s_row, s_w, s_h); // flatten the shared memory

	d_out[idx].x = 0;
	d_out[idx].y = 0;
	d_out[idx].z = 0;
	d_out[idx].w = 255;

	// let's play

	// load regular cells
	s_in[s_idx] = d_temp[idx];
	
	if(threadIdx.x < RAD) {
		// load column neighbors
		s_in[flatten(s_col - RAD, s_row, s_w, s_h)] = d_temp[flatten(col - RAD, row, w, h)];
		s_in[flatten(s_col + blockDim.x, s_row, s_w, s_h)] = d_temp[flatten(col + blockDim.x, row, w, h)];
	}

	if(threadIdx.y < RAD) {
		// load row neighbors
		s_in[flatten(s_col, s_row - RAD, s_w, s_h)] = d_temp[flatten(col, row - RAD, w, h)];
		s_in[flatten(s_col, s_row + blockDim.y, s_w, s_h)] = d_temp[flatten(col, row + blockDim.y, w, h)];
	}

	float dSq = ((col - bc.x) * (col - bc.x)) + (row - bc.y) * (row - bc.y);

	// if inside the pipe, set temp to source temperature and return

	if(dSq < bc.rad * bc.rad) {
		d_temp[idx] = bc.t_s;
		return;
	}

	// if outside the plate, set the temp to ambient
	if((col == 0) || (col == w - 1) || (row == 0) || (col + row < 0) || (col - row > w)) {
		d_temp[idx] = bc.t_a;
		return;
	}

	// if point is below ground, set temp to ground
	if(row == h - 1) {
		d_temp[idx] = bc.t_g;
		return;
	}

	__syncthreads();

	// for all remaining points, find temperatures and set colors

	float temp = 0.25f * (s_in[flatten(s_col - 1, s_row, s_w, s_h)] + s_in[flatten(s_col + 1, s_row, s_w, s_h)] + s_in[flatten(s_col, s_row - 1, s_w, s_h)] + s_in[flatten(s_col, s_row + 1, s_w, s_h)]);
	d_temp[idx] = temp;
	const unsigned char intensity = clip((int) temp);
	d_out[idx].x = intensity; // more heat -> more read
	d_out[idx].z = 255 - intensity; // less heat -> more blue
}

void kernelLauncher(uchar4 *d_out, float *d_temp, int w, int h, BC bc) {
	const dim3 blockSize(TX, TY);
	const dim3 gridSize(divUp(w, TX), divUp(h, TY));
	const size_t smSz = (TX + 2 * RAD) * (TY + 2 * RAD) * sizeof(float); // shared memory size
	tempKernel<<<gridSize, blockSize, smSz>>>(d_out, d_temp, w, h ,bc);
}

void resetTemperature(float *d_temp, int w, int h, BC bc) {
	const dim3 blockSize(TX, TY);
	const dim3 gridSize(divUp(w, TX), divUp(h, TY));
	resetKernel<<<gridSize, blockSize>>>(d_temp, w, h, bc);
}