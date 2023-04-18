__kernel void transpose(__global int* in, __global int* out, const int rows, const int cols) {
    int i = get_global_id(0);
    int j = get_global_id(1);
    out[j * rows + i] = in[i * cols + j];
}
