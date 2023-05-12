#define CL_TARGET_OPENCL_VERSION 300

#include "kernel_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>

#define MAX_SOURCE_SIZE (0x100000)

// Standard C implementation of QuickSort
void quicksort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            if (arr[j] <= pivot)
            {
                i++;
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        int temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        quicksort(arr, low, i);
        quicksort(arr, i + 2, high);
    }
}

int wmain()
{
    const int n = 1024;
    int arr[n];

    // Initialize input array with random values
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }

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
    cl_context context = clCreateContext(NULL, n_devices, &device_id, NULL, NULL, NULL);
    

    // Create a buffer object to store the input array
    cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(int), NULL, NULL);
    cl_mem output_buffer= clCreateBuffer(context,CL_MEM_READ_WRITE,n * sizeof(int),NULL,NULL);

    
    

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


    cl_kernel kernel = clCreateKernel(program, "quicksort", NULL);

    // Set the arguments for the kernel function
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&input_buffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&output_buffer);
    clSetKernelArg(kernel, 2, sizeof(int), (void *)&n);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, NULL, NULL);

    // Copy the input array to the buffer object
    clEnqueueWriteBuffer(command_queue, input_buffer, CL_TRUE, 0, n * sizeof(int), arr, 0, NULL, NULL);

    // Enqueue the kernel function for execution
    size_t global_item_size = n;
    cl_event event;
    clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, &event);

    //Wait for the kernel function to finish executing
    clWaitForEvents(1, &event);

    // Read the sorted array from the buffer object
    clEnqueueReadBuffer(command_queue, output_buffer, CL_TRUE, 0, n * sizeof(int), arr, 0, NULL, NULL);

    // Print the sorted array
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }

    //Get execution time
    cl_ulong start_time, end_time;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    double execution_time = (double)(end_time - start_time) / 1e9;
    printf("Execution time: %f seconds\n", execution_time);

    // Clean up our OpenCL resources
    clReleaseMemObject(input_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);

    return 0;
}
