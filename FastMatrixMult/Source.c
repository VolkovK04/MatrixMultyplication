#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <locale.h>
#include <conio.h>


typedef struct {
    int* data;
    int height;
    int width;
} Matrix;

typedef struct {
    Matrix parent;
    int positionX;
    int positionY;
    int height;
    int width;
} SubMatrix;

void multShtrassenImp(SubMatrix, SubMatrix, SubMatrix, int, int*);
void multRecurrentImp(SubMatrix, SubMatrix, SubMatrix, int);
void multNativeImp(SubMatrix, SubMatrix, SubMatrix, int);
void SubMatrix_SetElement(SubMatrix, int, int, int);

void nullCheck(void* ptr) {
    if (ptr == NULL) {
        printf("Out of memory!");
        exit(0);
    }
}

Matrix Matrix_Init(int n, int m) {
    int* ptr = (int*)malloc(n * m * sizeof(int));
    nullCheck(ptr);

    return (Matrix) {
        .height = n,
        .width = m,
        .data = ptr
    };
}

Matrix Matrix_InitFromBuffer(int n, int m, int* buffer) {
    return (Matrix) {
        .height = n,
        .width = m,
        .data = buffer
    };
}

void Matrix_Free(Matrix A) {
    free(A.data);
}

int Matrix_GetElement(Matrix A, int i, int j) {
    int ind = i * A.width + j;
    if (ind < A.height * A.width) {
        return A.data[ind];
    }
    return 0;
}

void Matrix_SetElement(Matrix A, int i, int j, int value) {
    int ind = i * A.width + j;
    if (ind < A.height * A.width) {
        A.data[i * A.width + j] = value;
    }
}

void Matrix_RandomFill(Matrix A, int maxValue) {
    for (int i = 0; i < A.height * A.width; i++) {
        A.data[i] = rand() % maxValue;
    }
}

void Matrix_ZeroFill(Matrix A) {
    memset(A.data, 0, A.height * A.width * sizeof(int));
}

void Matrix_Print(Matrix A) {
    for (int i = 0; i < A.height; i++) {
        for (int j = 0; j < A.width - 1; j++) {
            int value = Matrix_GetElement(A, i, j);
            printf("%4d, ", value);
        }
        int value = Matrix_GetElement(A, i, A.width - 1);
        printf("%4d\n", value);
    }
}

void SumSubMatrix(SubMatrix A, SubMatrix B, SubMatrix C) {
    for (int i = 0; i < A.height; i++) {
        for (int j = 0; j < A.width; j++) {
            SubMatrix_SetElement(C, i, j, SubMatrix_GetElement(A, i, j) + SubMatrix_GetElement(B, i, j));
        }
    }
}

void SubtractSubMatrix(SubMatrix A, SubMatrix B, SubMatrix C) {
    for (int i = 0; i < A.height; i++) {
        for (int j = 0; j < A.width; j++) {
            SubMatrix_SetElement(C, i, j, SubMatrix_GetElement(A, i, j) - SubMatrix_GetElement(B, i, j));
        }
    }
}

void CopySubMatrix(SubMatrix A, SubMatrix B) {
    for (int i = 0; i < A.height; i++) {
        for (int j = 0; j < A.width; j++) {
            SubMatrix_SetElement(B, i, j, SubMatrix_GetElement(A, i, j));
        }
    }
}

SubMatrix SubMatrix_Init(Matrix parent, int positionY, int positionX, int height, int width) {
    return (SubMatrix) {
        .parent = parent,
        .positionX = positionX,
        .positionY = positionY,
        .height = height,
        .width = width
    };
}

int SubMatrix_GetElement(SubMatrix A, int i, int j) {
    return Matrix_GetElement(A.parent, A.positionY + i, A.positionX + j);
}

void SubMatrix_SetElement(SubMatrix A, int i, int j, int value) {
    Matrix_SetElement(A.parent, A.positionY + i, A.positionX + j, value);
}

void multNative(Matrix A, Matrix B, Matrix C) {
    SubMatrix sA = SubMatrix_Init(A, 0, 0, A.height, A.width);
    SubMatrix sB = SubMatrix_Init(B, 0, 0, B.height, B.width);
    SubMatrix sC = SubMatrix_Init(C, 0, 0, C.height, C.width);
    multNativeImp(sA, sB, sC, 0);
}

void multNativeImp(SubMatrix A, SubMatrix B, SubMatrix C, int flagAdd) {
    int m = A.width;
    int n = A.height;
    int p = B.width;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            int buff;
            if (flagAdd) {
                buff = SubMatrix_GetElement(C, i, j);
            }
            else {
                buff = 0;
            }
            for (int k = 0; k < m; k++) {
                buff += SubMatrix_GetElement(A, i, k) * SubMatrix_GetElement(B, k, j);
            }
            SubMatrix_SetElement(C, i, j, buff);
        }
    }
}

void multRecurrent(Matrix X, Matrix Y, Matrix Z) {
    int k = 1;
    for (int n = max(max(X.height, X.width), Y.width) - 1; n > 0; n = n >> 1) {
        k = k << 1;
    }
    SubMatrix X1 = SubMatrix_Init(X, 0, 0, k, k);
    SubMatrix Y1 = SubMatrix_Init(Y, 0, 0, k, k);
    SubMatrix Z1 = SubMatrix_Init(Z, 0, 0, k, k);
    multRecurrentImp(X1, Y1, Z1, 0);

}

void multRecurrentImp(SubMatrix X1, SubMatrix Y1, SubMatrix Z1, int flagAdd) {
    if (Z1.positionX >= Z1.parent.width || Z1.positionY >= Z1.parent.height)
        return;
    int n = X1.height;
    if (n <= 64)
    {
        multNativeImp(X1, Y1, Z1, flagAdd);
        return;
    }
    int middle = X1.height / 2;
    Matrix X = X1.parent;
    Matrix Y = Y1.parent;
    Matrix Z = Z1.parent;

    SubMatrix A = SubMatrix_Init(X, X1.positionY, X1.positionX, middle, middle);
    SubMatrix B = SubMatrix_Init(X, X1.positionY, X1.positionX + middle, middle, middle);
    SubMatrix C = SubMatrix_Init(X, X1.positionY + middle, X1.positionX, middle, middle);
    SubMatrix D = SubMatrix_Init(X, X1.positionY + middle, X1.positionX + middle, middle, middle);

    SubMatrix E = SubMatrix_Init(Y, Y1.positionY, Y1.positionX, middle, middle);
    SubMatrix F = SubMatrix_Init(Y, Y1.positionY, Y1.positionX + middle, middle, middle);
    SubMatrix G = SubMatrix_Init(Y, Y1.positionY + middle, Y1.positionX, middle, middle);
    SubMatrix H = SubMatrix_Init(Y, Y1.positionY + middle, Y1.positionX + middle, middle, middle);

    SubMatrix Z11 = SubMatrix_Init(Z, Z1.positionY, Z1.positionX, middle, middle);
    SubMatrix Z12 = SubMatrix_Init(Z, Z1.positionY, Z1.positionX + middle, middle, middle);
    SubMatrix Z21 = SubMatrix_Init(Z, Z1.positionY + middle, Z1.positionX, middle, middle);
    SubMatrix Z22 = SubMatrix_Init(Z, Z1.positionY + middle, Z1.positionX + middle, middle, middle);

    multRecurrentImp(A, E, Z11, flagAdd);
    multRecurrentImp(B, G, Z11, 1);
    multRecurrentImp(A, F, Z12, flagAdd);
    multRecurrentImp(B, H, Z12, 1);
    multRecurrentImp(C, E, Z21, flagAdd);
    multRecurrentImp(D, G, Z21, 1);
    multRecurrentImp(C, F, Z22, flagAdd);
    multRecurrentImp(D, H, Z22, 1);
}

void multShtrassen(Matrix X, Matrix Y, Matrix Z) {
    int k = 1;
    for (int n = max(max(X.height, X.width), Y.width) - 1; n > 0; n = n >> 1) {
        k = k << 1;
    }
    SubMatrix X1 = SubMatrix_Init(X, 0, 0, k, k);
    SubMatrix Y1 = SubMatrix_Init(Y, 0, 0, k, k);
    SubMatrix Z1 = SubMatrix_Init(Z, 0, 0, k, k);
    int* buffer = (int*)malloc(3 * k * k * sizeof(int));
    nullCheck(buffer);
    multShtrassenImp(X1, Y1, Z1, 0, buffer);
    free(buffer);
}

void multShtrassenImp(SubMatrix X1, SubMatrix Y1, SubMatrix Z1, int flagAdd, int* buffer) {
    if (Z1.positionX >= Z1.parent.width || Z1.positionY >= Z1.parent.height)
        return;
    int n = X1.height;
    if (n <= 64)
    {
        multNativeImp(X1, Y1, Z1, flagAdd);
        return;
    }
    int middle = n / 2;
    Matrix X = X1.parent;
    Matrix Y = Y1.parent;
    Matrix Z = Z1.parent;

    SubMatrix A = SubMatrix_Init(X, X1.positionY, X1.positionX, middle, middle);
    SubMatrix B = SubMatrix_Init(X, X1.positionY, X1.positionX + middle, middle, middle);
    SubMatrix C = SubMatrix_Init(X, X1.positionY + middle, X1.positionX, middle, middle);
    SubMatrix D = SubMatrix_Init(X, X1.positionY + middle, X1.positionX + middle, middle, middle);

    SubMatrix E = SubMatrix_Init(Y, Y1.positionY, Y1.positionX, middle, middle);
    SubMatrix F = SubMatrix_Init(Y, Y1.positionY, Y1.positionX + middle, middle, middle);
    SubMatrix G = SubMatrix_Init(Y, Y1.positionY + middle, Y1.positionX, middle, middle);
    SubMatrix H = SubMatrix_Init(Y, Y1.positionY + middle, Y1.positionX + middle, middle, middle);

    SubMatrix Z11 = SubMatrix_Init(Z, Z1.positionY, Z1.positionX, middle, middle);
    SubMatrix Z12 = SubMatrix_Init(Z, Z1.positionY, Z1.positionX + middle, middle, middle);
    SubMatrix Z21 = SubMatrix_Init(Z, Z1.positionY + middle, Z1.positionX, middle, middle);
    SubMatrix Z22 = SubMatrix_Init(Z, Z1.positionY + middle, Z1.positionX + middle, middle, middle);

    int len = middle * middle;
    Matrix P = Matrix_InitFromBuffer(middle, middle, buffer);
    buffer += len;
    Matrix buff1 = Matrix_InitFromBuffer(middle, middle, buffer);
    buffer += len;
    Matrix buff2 = Matrix_InitFromBuffer(middle, middle, buffer);
    buffer += len;

    SubMatrix sP = SubMatrix_Init(P, 0, 0, middle, middle);
    SubMatrix sbuff1 = SubMatrix_Init(buff1, 0, 0, middle, middle);
    SubMatrix sbuff2 = SubMatrix_Init(buff2, 0, 0, middle, middle);
    // P1
    SubtractSubMatrix(F, H, sbuff1);
    multShtrassenImp(A, sbuff1, sP, 0, buffer);
    if (flagAdd) {
        SumSubMatrix(Z12, sP, Z12);
        SumSubMatrix(Z22, sP, Z22);
    }
    else {
        CopySubMatrix(sP, Z12);
        CopySubMatrix(sP, Z22);
    }
    // P4
    SubtractSubMatrix(G, E, sbuff1);
    multShtrassenImp(D, sbuff1, sP, 0, buffer);
    if (flagAdd) {
        SumSubMatrix(Z11, sP, Z11);
        SumSubMatrix(Z21, sP, Z21);
    }
    else {
        CopySubMatrix(sP, Z11);
        CopySubMatrix(sP, Z21);
    }
    // P2
    SumSubMatrix(A, B, sbuff1);
    multShtrassenImp(sbuff1, H, sP, 0, buffer);
    SumSubMatrix(Z12, sP, Z12);
    SubtractSubMatrix(Z11, sP, Z11);
    // P3
    SumSubMatrix(C, D, sbuff1);
    multShtrassenImp(sbuff1, E, sP, 0, buffer);
    SumSubMatrix(Z21, sP, Z21);
    SubtractSubMatrix(Z22, sP, Z22);
    // P5
    SumSubMatrix(A, D, sbuff1);
    SumSubMatrix(E, H, sbuff2);
    multShtrassenImp(sbuff1, sbuff2, sP, 0, buffer);
    SumSubMatrix(Z22, sP, Z22);
    SumSubMatrix(Z11, sP, Z11);
    // P6
    SubtractSubMatrix(B, D, sbuff1);
    SumSubMatrix(G, H, sP);
    multShtrassenImp(sbuff1, sP, Z11, 1, buffer);
    // P7
    SubtractSubMatrix(C, A, sbuff1);
    SumSubMatrix(E, F, sP);
    multShtrassenImp(sbuff1, sP, Z22, 1, buffer);
}

#define MAX_VALUE 10

double sampleMean(int* arr, int len) {
    int sum = 0;
    for (int i = 0; i < len; i++) {
        sum += arr[i];
    }
    double result = (double)sum / len;
    return result;
}

double sampleStandardDeviation(int* arr, int len) {
    double mean = sampleMean(arr, len);
    double sum = 0;
    for (int i = 0; i < len; i++) {
        double buff = arr[i] - mean;
        sum += buff * buff;
    }
    return sqrt(sum) / len;
}

double geometricMean(int* arr, int len) {
    double product = 1;
    for (int i = 0; i < len; i++) {
        product *= arr[i];
    }
    return pow(product, 1.0 / len);
}

void printTimes(FILE* file, int* arr,int len) {
    fprintf(file, "%.2f;%.2f;%.2f;", sampleMean(arr, len), sampleStandardDeviation(arr, len), geometricMean(arr, len));
}

int main() {
    setlocale(LC_ALL, "ru_RU.utf8");
    unsigned int seed = 777; // time(NULL)
    srand(seed);
    FILE* file = fopen("result.csv", "w");

    int k = 10;
    int* tNative = (int*)malloc(k * sizeof(int));
    nullCheck(tNative);
    int* tRecurrent = (int*)malloc(k * sizeof(int));
    nullCheck(tRecurrent);
    int* tShtrassen = (int*)malloc(k * sizeof(int));
    nullCheck(tShtrassen);

    fprintf(file, "N;Native;;;Recurrent;;;Shtrassen\n");
    fprintf(file, ";sampleMean;sampleStandardDeviation;geometricMean;sampleMean;sampleStandardDeviation;geometricMean;sampleMean;sampleStandardDeviation;geometricMean\n");

    for (int n = 64; n <= 1024; n += 64)
    {
        printf("%d\n", n);
        Matrix A = Matrix_Init(n, n);
        Matrix B = Matrix_Init(n, n);
        Matrix C = Matrix_Init(n, n);




        for (int i = 0; i < k; i++) {
            Matrix_RandomFill(A, MAX_VALUE);
            Matrix_RandomFill(B, MAX_VALUE);

            clock_t time = clock();
            multNative(A, B, C);
            tNative[i] = (int)(clock() - time);
            time = clock();
            multRecurrent(A, B, C);
            tRecurrent[i] = (int)(clock() - time);
            time = clock();
            multShtrassen(A, B, C);
            tShtrassen[i] = (int)(clock() - time);

        }
        Matrix_Free(A);
        Matrix_Free(B);
        Matrix_Free(C);
        fprintf(file, "%d;", n);
        printTimes(file, tNative, k);
        printTimes(file, tRecurrent, k);
        printTimes(file, tShtrassen, k);
        fprintf(file, "\n");
    }
    free(tNative);
    free(tRecurrent);
    free(tShtrassen);

    fclose(file);



}