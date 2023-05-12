#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int L[n1], R[n2];

    for (i = 0; i < n1; i++) {
        L[i] = arr[l + i];
    }
    for (j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
} /*A merge függvény a tömb két rendezett altábláját veszi, és egyetlen rendezett altáblává egyesíti őket.*/

void mergesort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergesort(arr, l, m);
        mergesort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}
/*A mergesort függvény rekurzívan meghívja önmagát a tömb rendezéséhez. 
Először két részre osztja a tömböt, és a mergesort függvény segítségével külön-külön rendezi őket. 
Ezután a két rendezett felet a merge függvény segítségével egyesíti.*/

int main() {
    int n;
    printf("Enter the number of elements in the array: "); /*Először a felhasználó megadja a tömb elemeinek számát.*/
    scanf("%d", &n);

    int arr[n];
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }/*Ezután létrehozunk n véletlenszerű elemet, és eltároljuk őket a tömbben.*/

    clock_t start_time = clock();
    mergesort(arr, 0, n - 1);
    clock_t end_time = clock();
    /*Ezután meghívjuk a mergesort függvényt a tömb rendezéséhez, és az clock függvénnyel mérjük az eltelt időt.*/

    printf("Sorted array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nTime taken: %lf seconds\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);

    return 0;
}
