LDFLAGS += -lGL -lglfw -lGLEW -lglut -lGLU

NVCC = /usr/lib/nvidia-cuda-toolkit/bin/nvcc
NVCC_FLAGS = -g -G -Xcompiler "-Wall -Wno-deprecated-declarations"

all: main
main: main.o kernel.o
	$(NVCC) $^ -o $@ $(LDFLAGS)
main.o: main.cpp kernel.h interactions.h
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@ 
kernel.o: tempKernel.cu kernel.h
	$(NVCC) $(NVCC_FLAGS) -c $< -o $@
