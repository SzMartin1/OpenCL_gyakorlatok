__kernel void sum_columns(__global int* mat, __global int* result, const int rows, const int cols) {
    int gid_x = get_global_id(0);

    int sum = 0;
    for (int i = 0; i < rows; i++) {
        sum += mat[i * cols + gid_x];
    }
    result[gid_x] = sum;
}