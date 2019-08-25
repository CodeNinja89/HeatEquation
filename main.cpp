#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES // required... fakes forwad declaration for GL
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <cuda_gl_interop.h>
#include "interactions.h"

#define ITERS_PER_RENDER 50

GLuint pbo = 0; // OpenGL Pixel Buffer Object
GLuint tex = 0; // OpenGL texture object

struct cudaGraphicsResource *cuda_pbo_resource;

void render() {
	uchar4 *d_out = 0;
	int i = 0;

	cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
	cudaGraphicsResourceGetMappedPointer((void **)&d_out, NULL, cuda_pbo_resource);

	for(i = 0; i < ITERS_PER_RENDER; i++) {
		kernelLauncher(d_out, d_temp, W, H, bc); // launch kernel for each iteration... live display!
	}

	cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

	char title[128];
	sprintf(title, "Temperature Visualizer - Iterations: %4d, T_s = %3.0f, T_a = %3.0f, T_g = %3.0f", iterationCount, bc.t_s, bc.t_a, bc.t_g);
	glutSetWindowTitle(title);
}

void drawTexture() {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0, H);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(W, H);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(W, 0);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void display() {
	render();
	drawTexture();
	glutSwapBuffers();
}

void initGLUT(int *argc, char **argv) {
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutCreateWindow(TITLE_STRING);
}

void initPixelBuffer() {
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, 4 * W * H * sizeof(GLubyte), 0, GL_STREAM_DRAW);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard);
}

void exitfunc() {
	if(pbo) {
		cudaGraphicsUnregisterResource(cuda_pbo_resource);
		glDeleteBuffers(1, &pbo);
		glDeleteTextures(1, &tex);
	}
}

int main(int argc, char **argv) {
	cudaMalloc(&d_temp, W * H * sizeof(float));
	resetTemperature(d_temp, W, H, bc);
	/*printInstructions();*/
	initGLUT(&argc, argv);
	gluOrtho2D(0, W, H, 0);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	initPixelBuffer();
	glutMainLoop();
	atexit(exitfunc);
	return 0;
}

