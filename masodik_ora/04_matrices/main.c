#include "kernel_loader.h"
#include "clext.h"
#define CL_TARGET_OPENCL_VERSION 220

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>


const int ROWS = 4;
const int COLS = 4;

int main(void)
{
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
    const char* kernel_code = load_kernel_source("kernels/kernel3.cl", &error_code);
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
    cl_kernel kernel = clCreateKernel(program, "sum_columns", NULL);

	int mat[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	int transposed_mat[ROWS * COLS];
	
    // Create the host buffer and initialize it
    //mat = (int*) malloc(ROWS*COLS*sizeof(int));
    //transposed_mat = (int*) malloc(ROWS*COLS*sizeof(int));

    
    // Create the device buffer // Dobozoló 
    cl_mem a_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, ROWS*COLS*sizeof(int), NULL, NULL);
    cl_mem b_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, ROWS*COLS*sizeof(int), NULL, NULL);
	
    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&a_buffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&b_buffer);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)(&ROWS));
	clSetKernelArg(kernel, 3, sizeof(int), (void*)(&COLS));

    // Create the command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, NULL, NULL);

    // Host buffer -> Device buffer
    clEnqueueWriteBuffer(
        command_queue,
        a_buffer,
        CL_FALSE,
        0,
        sizeof(int) * ROWS * COLS, //changed
        mat, //changed
        0,
        NULL,
        NULL
    );

    // Size specification
    size_t local_work_size[2] = {1,1}; 
    size_t global_work_size[2] = {COLS, ROWS}; 

    // Apply the kernel on the range
    clEnqueueNDRangeKernel(
        command_queue,
        kernel,
        2,
        NULL,
        &global_work_size,
        &local_work_size,
        0,
        NULL,
        NULL
    );
    clFinish(command_queue);
    // Host buffer <- Device buffer
    //OUTPUT
    clEnqueueReadBuffer(
        command_queue,
        b_buffer,
        CL_TRUE,
        0,
        sizeof(int) * ROWS * COLS,
        transposed_mat,
        0,
        NULL,
        NULL
    );
    printf("Result matrix is \n");
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++){
			printf("%d ", transposed_mat[i * COLS +j]);
		}
		printf("\n");
	}
    // Release the resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseContext(context);
    clReleaseDevice(device_id);

}