#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}/*A swap függvényt a tömb két elemének felcserélésére használjuk.*/

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}/*A 'partition' függvény a tömb utolsó elemét veszi pivotnak, és az összes kisebb elemet a pivottól balra, az összes nagyobb elemet pedig a pivottól jobbra helyezi. Visszaadja a pivot elem indexét.*/

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}/*A quicksort függvény rekurzívan hívja meg önmagát a tömb rendezéséhez. Először meghívja a partíció függvényt, hogy a tömböt két félre ossza egy pivot elem körül, majd a tömb bal és jobb felét külön-külön rendezi.*/

int main() {
    int n;
    printf("Enter the number of elements in the array: ");
    scanf("%d", &n); /*A felhasználó megadja a tömb elemeinek számát.*/

    int arr[n];
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100;
    }/*Ezután létrehozunk n véletlenszerű elemet, és eltároljuk őket a tömbben.*/

    clock_t start_time = clock(); /*Időmérés kezdete*/
    quicksort(arr, 0, n - 1); /*Ezután meghívjuk a quicksort függvényt a tömb rendezéséhez.*/
    clock_t end_time = clock();/*Időmérés vége*/

    printf("Sorted array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nTime taken: %lf seconds\n", ((double)(end_time - start_time)) / CLOCKS_PER_SEC);

    return 0;
}
