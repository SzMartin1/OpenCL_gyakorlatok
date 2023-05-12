__kernel void quicksort(__global int *arr, const int n) {
    int low = 0;
    int high = n - 1;

    // Stack to keep track of subarrays to be sorted
    __local int stack[32];
    int top = -1;

    // Push initial subarray onto the stack
    stack[++top] = low;
    stack[++top] = high;

    // Loop until stack is empty
    while (top >= 0) {
        // Pop subarray from stack
        high = stack[top--];
        low = stack[top--];

        // Partition the subarray
        int pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++) {
            if (arr[j] <= pivot) {
                i++;
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        int temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        // Push left subarray onto stack
        if (i - 1 > low) {
            stack[++top] = low;
            stack[++top] = i - 1;
        }

        // Push right subarray onto stack
        if (i + 1 < high) {
            stack[++top] = i + 1;
            stack[++top] = high;
        }
    }
}
