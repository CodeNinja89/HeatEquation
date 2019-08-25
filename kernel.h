#ifndef KERNEL_H
#define KERNEL_H

#include <cuda.h>
#include <cuda_runtime.h>

struct uchar4;
struct int2;

typedef struct {
	int x, y; // location of the pipe center
	float rad; // radius of the pipe
	float t_s, t_a, t_g; // air, ground, and source temperature
} BC; // boundary conditions

void kernelLauncher(uchar4 *d_out, float *d_temp, int w, int h, BC bc);
void resetTemperature(float *d_temp, int w, int h, BC bc);

#endif