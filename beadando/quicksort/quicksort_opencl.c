#define CL_TARGET_OPENCL_VERSION 300

#include "kernel_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>
#include <stddef.h>

#define MAX_SOURCE_SIZE (0x100000)

// Function to get the error string corresponding to the error code
const char* getErrorString(cl_int err) {
    switch (err) {
        case CL_SUCCESS: return "Success";
        case CL_DEVICE_NOT_FOUND: return "Device not found";
        case CL_DEVICE_NOT_AVAILABLE: return "Device not available";
        case CL_COMPILER_NOT_AVAILABLE: return "Compiler not available";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "Memory object allocation failure";
        case CL_OUT_OF_RESOURCES: return "Out of resources";
        case CL_OUT_OF_HOST_MEMORY: return "Out of host memory";
        case CL_INVALID_VALUE: return "Invalid value";
        case CL_INVALID_DEVICE_TYPE: return "Invalid device type";
        case CL_INVALID_PLATFORM: return "Invalid platform";
        case CL_INVALID_DEVICE: return "Invalid device";
        case CL_INVALID_CONTEXT: return "Invalid context";
        case CL_INVALID_QUEUE_PROPERTIES: return "Invalid queue properties";
        case CL_INVALID_COMMAND_QUEUE: return "Invalid command queue";
        case CL_INVALID_HOST_PTR: return "Invalid host pointer";
        case CL_INVALID_MEM_OBJECT: return "Invalid memory object";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "Invalid image format descriptor";
        case CL_INVALID_IMAGE_SIZE: return "Invalid image size";
        case CL_INVALID_SAMPLER: return "Invalid sampler";
        case CL_INVALID_BINARY: return "Invalid binary";
        case CL_INVALID_BUILD_OPTIONS: return "Invalid build options";
        case CL_INVALID_PROGRAM: return "Invalid program";
        case CL_INVALID_PROGRAM_EXECUTABLE: return "Invalid program executable";
        case CL_INVALID_KERNEL_NAME: return "Invalid kernel name";
        case CL_INVALID_KERNEL_DEFINITION: return "Invalid kernel definition";
        case CL_INVALID_KERNEL: return "Invalid kernel";
        case CL_INVALID_ARG_INDEX: return "Invalid argument index";
        case CL_INVALID_ARG_VALUE: return "Invalid argument value";
        case CL_INVALID_ARG_SIZE: return "Invalid argument size";
        case CL_INVALID_KERNEL_ARGS: return "Invalid kernel arguments";
        case CL_INVALID_WORK_DIMENSION: return "Invalid work dimension";
        case CL_INVALID_WORK_GROUP_SIZE: return "Invalid work group size";
        case CL_INVALID_WORK_ITEM_SIZE: return "Invalid work item size";
        case CL_INVALID_GLOBAL_OFFSET: return "Invalid global offset";
        case CL_INVALID_EVENT_WAIT_LIST: return "Invalid event wait list";
        case CL_INVALID_EVENT: return "Invalid event";
        case CL_INVALID_OPERATION: return "Invalid operation";
        case CL_INVALID_GL_OBJECT: return "Invalid OpenGL object";
        case CL_INVALID_BUFFER_SIZE: return "Invalid buffer size";
        case CL_INVALID_MIP_LEVEL: return "Invalid mip-map level";
        case CL_INVALID_GLOBAL_WORK_SIZE: return "Invalid global work size";
        case CL_INVALID_PROPERTY: return "Invalid property";
        default: return "Unknown error code";
    }
}

int wmain()
{
    const int n = 256;
    
    

    cl_int err;
    int error_code;

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
    cl_context context = clCreateContext(NULL, n_devices, &device_id, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("[ERROR] Error creating context. Error code: %d\n", err);
        return 0;
    }
    
     

    // Create a kernel function to implement QuickSort
    const char* kernel_code = load_kernel_source("kernels/quicksort_kernel.cl", &error_code);
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


    cl_kernel kernel = clCreateKernel(program, "quicksort", &err);
    if (err != CL_SUCCESS) {
        printf("[ERROR] Error creating kernel. Error code: %d\n", err);
        return 0;
    }

    //Creating an array which will contain random numbers
    srand(time(NULL));
    int* input_arr = (int*)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) {
        input_arr[i] = rand() % 100;
    }

    // Create a buffer object to store the input and output array
    cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &err);
    if (err != CL_SUCCESS) {
        printf("[ERROR] Error creating input buffer. Error code: %d\n", err);
        return 0;
    }

    // Set the arguments for the kernel function
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(int), (void *)&n);
    if(err != CL_SUCCESS){
        printf("Error setting the kernel arguments. Error code: %d\n", &err);
        printf("Error message: %s\n", getErrorString(err));
        return 0;
    }

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
    if(err != CL_SUCCESS){
        printf("Error in the command queue! Error code: %d\n",&err);
        return 0;
    }

    // Copy the input array to the buffer object
    err = clEnqueueWriteBuffer(
    command_queue, 
    input_buffer, 
    CL_TRUE, 
    0, 
    n * sizeof(int), 
    input_arr, 
    0, 
    NULL, 
    NULL
    );
    if(err != CL_SUCCESS){
        printf("Error copying input data to buffer! Error code: %d\n",&err);
        return 0;
    }
    // Enqueue the kernel function for execution
    //size_t local_work_size = 256;
    size_t global_work_size = n;

    cl_event event;

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
        printf("Error in enqueueing the kernel. Error code: %d\n",&err);
        return 0;
    }
    //Wait for the kernel function to finish executing
    clWaitForEvents(1, &event);
    clFinish(command_queue);

    // Read the sorted array from the buffer object
    int* output_arr = (int*)malloc(sizeof(int)* n);
    err = clEnqueueReadBuffer(
    command_queue, 
    input_buffer, 
    CL_TRUE, 
    0, 
    n * sizeof(int), 
    output_arr, 
    0, 
    NULL, 
    NULL
    );
    if(err != CL_SUCCESS){
        printf("Error reading output data from buffer! Error code: %d\n",&err);
        return 0;
    }

    // Print the sorted array
    for (int i = 0; i < n; i++)
    {
        printf("%d ", output_arr[i]);
    }

    //Get execution time
    cl_ulong start_time, end_time;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    double execution_time = end_time-start_time;
    printf("Execution time: %0.3f milliseconds\n", execution_time / 1000000.0);

    // Clean up our OpenCL resources
    free(input_arr);
    free(output_arr);
    clReleaseMemObject(input_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);

    return 0;
}
