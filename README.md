# HeatEquation
CUDA/OpenGl interop for simulating steady-state heat distribution on a 2-D plate. Adapted from the book CUDA for Engineers by Stroti and Yurtoglu (https://www.amazon.fr/gp/product/013417741X?pf_rd_p=61e3aca3-2f4c-4ed4-8b56-08aa65c1d16f&pf_rd_r=5SKPWSN5NCC3SAXD8GEG)

REQUIRED:
  PLATFORM: Linux Mint 19.
  - CUDA compliant GPU (using NVIDIA GeForce 940MX)
  - CUDA toolkit. If using Linux, download using Synaptic Package Manageror any package manager of your choice. Details: https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)
  - OpenGL. Install using your favorite package manager (details: https://medium.com/@Plimsky/how-to-install-a-opengl-environment-on-ubuntu-e3918cf5ab6c)
  - gcc and g++ and whole GNU toolkit (make and friends). Comes in the standard installation of Linux. If not, download using your favorite package manager.
  
use the Makefile to compile the code. Note that you will have to edit the Makefile according to your installation paths of the NVCC compiler.
A bit of basic physics/mathematics is required to understand the actual implementation.
