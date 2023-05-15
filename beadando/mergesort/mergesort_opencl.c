#include "kernel_loader.h"

#define CL_TARGET_OPENCL_VERSION 300

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>

// OpenCL kernel code for merge sort

int wmain() {
  // Set up OpenCL environment (context, command queue, program)
    cl_int err;
    int error_code;
    const int n = 256;

    // Get platform
    cl_uint n_platforms;
	cl_platform_id platform_id;

    err = clGetPlatformIDs(1, &platform_id, &n_platforms);
	if (err != CL_SUCCESS) {
		printf("[ERROR] Error calling clGetPlatformIDs. Error code: %d\n", err);
		return 0;
	}

    // Get device
	cl_device_id device_id;
	cl_uint n_devices;

	err = clGetDeviceIDs(
		platform_id,
		CL_DEVICE_TYPE_GPU,
		1,
		&device_id,
		&n_devices
	);

	if (err != CL_SUCCESS) {
		printf("[ERROR] Error calling clGetDeviceIDs. Error code: %d\n", err);
		return 0;
	}


    // Create a context for our OpenCL program
    cl_context context = clCreateContext(NULL, n_devices, &device_id, NULL, NULL, NULL);
    
     

    // Create a kernel function to implement QuickSort
    const char* kernel_code = load_kernel_source("kernels/mergesort_kernel.cl", &error_code);
    if (error_code != 0) {
        printf("Source code loading error!\n");
        return 0;
    }


    cl_program program = clCreateProgramWithSource(context, 1, &kernel_code, NULL, NULL);

    const char options[] = "";

    err = clBuildProgram(
        program,
        1,
        &device_id,
        options,
        NULL,
        NULL
    );
    if (err != CL_SUCCESS) {
        printf("Build error! Code: %d\n", err);
        size_t real_size;
        err = clGetProgramBuildInfo(
            program,
            device_id,
            CL_PROGRAM_BUILD_LOG,
            0,
            NULL,
            &real_size
        );
        char* build_log = (char*)malloc(sizeof(char) * (real_size + 1));
        err = clGetProgramBuildInfo(
            program,
            device_id,
            CL_PROGRAM_BUILD_LOG,
            real_size + 1,
            build_log,
            &real_size
        );
        // build_log[real_size] = 0;
        printf("Real size : %d\n", real_size);
        printf("Build log : %s\n", build_log);
        free(build_log);
        return 0;
    }
    // Input array
    srand(time(NULL));
    int* input_arr = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        input_arr[i] = rand() % 100;
    }
    size_t tempSize = n * sizeof(int);
    int* tempHost = (int*)malloc(tempSize);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, NULL);

      // Allocate device memory for input and output arrays
    cl_mem d_input = clCreateBuffer(context, CL_MEM_READ_ONLY, tempSize, NULL, &err);
    cl_mem d_output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, tempSize, NULL, NULL);
    
    if (err != CL_SUCCESS) {
    printf("Failed to create temp buffer on the device.\n");
    // Handle the error
   }

    // Copy input array from host to device
    clEnqueueWriteBuffer(command_queue, d_input, CL_TRUE, 0, tempSize, tempHost, 0, NULL, NULL);

    // Create the merge sort kernel
      cl_kernel kernel = clCreateKernel(program, "mergeSort", &err);
      if(err!=CL_SUCCESS){
        printf("Failed to create kernel. Error code: %d\n", &err);
      }

    // Set the arguments of the merge sort kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_output);
    err |= clSetKernelArg(kernel, 2, sizeof(int), (void *)&n);
    if(err!=CL_SUCCESS)
    {
        printf("Failed to set the kernel argument for temp buffer. Error code: %d\n",&err);
        return 0;

    }

    // Enqueue the kernel function for execution
    size_t global_work_size = n;

    cl_event event;

    // Enqueue the merge sort kernel for execution
    err = clEnqueueNDRangeKernel(
        command_queue, 
        kernel, 
        1, 
        NULL, 
        &global_work_size, 
        NULL, 
        0, 
        NULL, 
        &event
        );
    
    if(err!=CL_SUCCESS){
        printf("Failed to execute the kernel. Error code: %d\n", &err);
        return 0;
    }
    
    clWaitForEvents(1, &event);
    clFinish(command_queue);

    // Read the sorted array from device to host
    err = clEnqueueReadBuffer(command_queue, d_input, CL_TRUE, 0, tempSize, tempHost, 0, NULL, NULL);
    if(err!=CL_SUCCESS){
        printf("Failed to transfer sorted data from device to host. Error code: %d\n",&err);
    }

    // Print the sorted array
      for (int i = 0; i < sizeof(input_arr) / sizeof(input_arr[0]); i++) {
        printf("%d ", tempHost[i]);
    }

     //Get execution time
    cl_ulong start_time, end_time;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    double execution_time = end_time-start_time;
    printf("Execution time: %0.3f milliseconds\n", execution_time / 1000000.0);

    // Clean up our OpenCL resources
    free(tempHost);
    clReleaseMemObject(d_input);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);
}