__kernel void sum_rows(__global int* mat, __global int* result, const int rows, const int cols) {
    int gid_y = get_global_id(0);

    int sum = 0;
    for (int i = 0; i < cols; i++) {
        sum += mat[gid_y * cols + i];
    }
    result[gid_y] = sum;
}