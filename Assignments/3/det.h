/*
    Code provided by Remi L. for CSC 357 Assignment 3
    rlos at calpoly.edu
*/

#include "stdio.h"
#include <stdlib.h>

const int MAT_NUM_COLS = 3;
const int MAT_NUM_ROWS = 3;
const int MAT_SIZE     = MAT_NUM_COLS * MAT_NUM_ROWS;
const int MAT_VALUES[] = {7, -4, 2, 3, 1, -5, 2, 2, -5};

// Calculates the minor of an nxn matrix
float *minor(const float *mat, int skip_row, int skip_col, int cols, int rows) {
    int minor_cols = cols - 1;
    int minor_rows = rows - 1;
    float *m       = (float *)malloc(minor_cols * minor_rows * sizeof(float));
    int k          = 0;
    for (int i = 0; i < rows; ++i) {
        if (i == skip_row)
            continue;
        for (int j = 0; j < cols; ++j) {
            if (j == skip_col)
                continue;
            m[k++] = mat[i * cols + j];
        }
    }
    return m;
}

// Calculates the determinant of an nxn matrix
// See: Laplace expansion (aka cofactor expansion) from
// https://en.wikipedia.org/wiki/Laplace_expansion
float det(const float *mat, int cols, int rows) {
    // base case 1: 1x1 (A[0])
    if (rows == 1 && cols == 1)
        return mat[0];

    // base case 2: 2x2 (ab - cd)
    if (rows == 2 && cols == 2)
        return mat[0] * mat[3] - mat[1] * mat[2];

    float determinant = 0.0f;

    // expands along the first row
    for (int j = 0; j < cols; ++j) {
        int sign    = (j % 2 == 0) ? 1 : -1;
        float coeff = mat[j] * sign;
        float *m    = minor(mat, 0, j, cols, rows);
        determinant += coeff * det(m, cols - 1, rows - 1);
        free(m);
    }

    return determinant;
}

// int main(int argc, char *argv[]) {
//     float *A = (float *)malloc(sizeof(float) * MAT_SIZE);

//     for (int i = 0; i < MAT_SIZE; i++)
//         A[i] = MAT_VALUES[i];

//     float det_A = det(A, MAT_NUM_COLS, MAT_NUM_ROWS);
//     printf("det(A) = %.2f\n", det_A);
//     free(A);
// }