#include "kernel_loader.h"
#include "clext.h"
#define CL_TARGET_OPENCL_VERSION 220

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

const int ARRAY_SIZE = 1000;

int main(void)
{
	//Creating the array filled with random numbers
	int array[ARRAY_SIZE];
	srand(time(0));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % 101;
    }
	
    cl_int error_code; //For error code handling

    // Get platform
    cl_uint n_platforms;
	cl_platform_id platform_id;
    error_code = clGetPlatformIDs(1, &platform_id, &n_platforms);
	if (error_code != CL_SUCCESS) {
		printf("[ERROR] Error calling clGetPlatformIDs. Error code: %d\n", error_code);
		return 0;
	}

    // Get device
	cl_device_id device_id;
	cl_uint n_devices;
	error_code = clGetDeviceIDs(
		platform_id,
		CL_DEVICE_TYPE_GPU,
		1,
		&device_id,
		&n_devices
	);
	if (error_code != CL_SUCCESS) {
		printf("[ERROR] Error calling clGetDeviceIDs. Error code: %d\n", error_code);
		return 0;
	}

    // Create OpenCL context
    cl_context context = clCreateContext(NULL, n_devices, &device_id, NULL, NULL, NULL);

    // Build the program
    const char* kernel_code = load_kernel_source("kernels/gyakorisag.cl", &error_code);
    if (error_code != 0) {
        printf("Source code loading error!\n");
        return 0;
    }
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_code, NULL, NULL);
    error_code = clBuildProgram(
        program,
        1,
        &device_id,
        NULL,
        NULL,
        NULL
    );
    //Error handling in case of faulty program building
    if (error_code != CL_SUCCESS) {
        printf("Build error! Code: %d\n", error_code);
		
		char* error_string = clGetErrorString(error_code);
		printf("Build error code in string: %s\n",error_string);
		
        size_t real_size;
        error_code = clGetProgramBuildInfo(
            program,
            device_id,
            CL_PROGRAM_BUILD_LOG,
            0,
            NULL,
            &real_size
        );
        char* build_log = (char*)malloc(sizeof(char) * (real_size + 1));
        error_code = clGetProgramBuildInfo(
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
	
	cl_kernel kernel = clCreateKernel(program,"gyakorisag_szamitas",NULL);
	
	//Creating the input and output buffers
	cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, ARRAY_SIZE * sizeof(int), NULL, NULL);
    cl_mem output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 101 * sizeof(int), NULL, NULL);
	
	//Setting the kernel arguments
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input_buffer);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output_buffer);
	clSetKernelArg(kernel, 2, sizeof(int), (void*)&ARRAY_SIZE);
	
	//Creating command queue
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, NULL, NULL);
	
	clEnqueueWriteBuffer(
        command_queue,
        input_buffer,
        CL_TRUE,
        0,
        sizeof(int) * ARRAY_SIZE, //changed
        array, //changed
        0,
        NULL,
        NULL
    );
	
	//Executing the kernel
	size_t global_work_size = 101;
    size_t local_work_size = 1;
	clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
	
	//Finishing the execution of the kernel
	clFinish(kernel);
	
	// Read results from output buffer
    //int frequency[101];
    clEnqueueReadBuffer(command_queue, output_buffer, CL_TRUE, 0, ARRAY_SIZE * sizeof(int), array, 0, NULL, NULL);
	
	// Print frequency of each integer
    for (int i = 0; i < 101; i++) {
    printf("%d occurs %d times\n", i, array[i]);
    }
	
	//Cleaning up the mess
	clReleaseMemObject(input_buffer);
    clReleaseMemObject(output_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
	
	
}