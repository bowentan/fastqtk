#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const** argv) {
    // double (*arr)[4];
    double **arr;
    arr = malloc(100 * sizeof(double[4]));
    // for (int i = 0; i < 100; i++) {
    //     memset(arr[i], 0, sizeof(double) * 4);
        // for (int j = 0; j < 4; j++) {
        //     arr[i][j] = i * j;
        // }
    // }
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}
