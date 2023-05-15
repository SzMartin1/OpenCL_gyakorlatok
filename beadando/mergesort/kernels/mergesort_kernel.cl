// Merge two sorted sub-arrays into a single sorted array
void merge(__global int* arr, __global int* temp, const int start, const int mid, const int end) {
  const int leftLength = mid - start + 1;
  const int rightLength = end - mid;

  int i, j, k;

  // Copy data to temporary arrays
  for (i = 0; i < leftLength; i++)
    temp[i] = arr[start + i];
  for (j = 0; j < rightLength; j++)
    temp[leftLength + j] = arr[mid + 1 + j];

  // Merge the temporary arrays back into arr[start..end]
  i = 0;  // Initial index of first sub-array
  j = leftLength;  // Initial index of second sub-array
  k = start;  // Initial index of merged sub-array

  while (i < leftLength && j < leftLength + rightLength) {
    if (temp[i] <= temp[j]) {
      arr[k] = temp[i];
      i++;
    } else {
      arr[k] = temp[j];
      j++;
    }
    k++;
  }

  // Copy the remaining elements of left[] if any
  while (i < leftLength) {
    arr[k] = temp[i];
    i++;
    k++;
  }

  // Copy the remaining elements of right[] if any
  while (j < leftLength + rightLength) {
    arr[k] = temp[j];
    j++;
    k++;
  }
}

// Recursive merge sort function
void mergeSortRecursive(__global int* arr, __global int* temp, const int start, const int end) {
  if (start < end) {
    const int mid = start + (end - start) / 2;
    mergeSortRecursive(arr, temp, start, mid);
    mergeSortRecursive(arr, temp, mid + 1, end);
    merge(arr, temp, start, mid, end);
  }
}

// Call the recursive merge sort function
__kernel void mergeSort(__global int* arr, __global int* temp, const int length) {
  mergeSortRecursive(arr, temp, 0, length - 1);
}
