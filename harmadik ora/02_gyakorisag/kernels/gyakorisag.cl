__kernel void gyakorisag_szamitas(__global const int* input, __global int* output, int size)
{
    int index = get_global_id(0);
    int value = input[index];
    atomic_inc(&output[value]);
}
