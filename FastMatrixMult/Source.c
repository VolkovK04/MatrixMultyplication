#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>


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

void multShtrassenImp(SubMatrix, SubMatrix, SubMatrix);
void multRecurrentImp(SubMatrix, SubMatrix, SubMatrix);
void multNativeImp(SubMatrix, SubMatrix, SubMatrix);
void SubMatrix_SetElement(SubMatrix, int, int, int);

void nullCheck(void* ptr) {
    if (ptr == NULL) {
        printf("Out of memory!");
        exit(0);
    }
}

Matrix Matrix_Init(int n, int m) {
    int* ptr = (int*)calloc(n * m, sizeof(int));
    nullCheck(ptr);

    return (Matrix) {
        .height = n,
            .width = m,
            .data = ptr
    };
}

void Matrix_Free(Matrix A) {
    free(A.data);
}

int Matrix_GetElement(Matrix A, int i, int j) {
    if (i >= 0 && i < A.height && j >= 0 && j < A.width) {
        return A.data[i * A.width + j];
    }
    else {
        return 0;
    }
}

void Matrix_SetElement(Matrix A, int i, int j, int value) {
    if (i >= 0 && i < A.height && j >= 0 && j < A.width) {
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
    SubMatrix A1 = SubMatrix_Init(A, 0, 0, A.height, A.width);
    SubMatrix B1 = SubMatrix_Init(B, 0, 0, B.height, B.width);
    SubMatrix C1 = SubMatrix_Init(C, 0, 0, C.height, C.width);
    multNativeImp(A1, B1, C1);
}

void multNativeImp(SubMatrix A, SubMatrix B, SubMatrix C) {
    int m = A.width;
    int n = A.height;
    int p = B.width;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            //int buff = 0;
            int buff = SubMatrix_GetElement(C, i, j);
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
    multRecurrentImp(X1, Y1, Z1);

}

void multRecurrentImp(SubMatrix X1, SubMatrix Y1, SubMatrix Z1) {
    int n = X1.height;
    if (n <= 16)
    {
        multNativeImp(X1, Y1, Z1);
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

    multRecurrentImp(A, E, Z11);
    multRecurrentImp(B, G, Z11);
    multRecurrentImp(A, F, Z12);
    multRecurrentImp(B, H, Z12);
    multRecurrentImp(C, E, Z21);
    multRecurrentImp(D, G, Z21);
    multRecurrentImp(C, F, Z22);
    multRecurrentImp(D, H, Z22);
}

void multShtrassen(Matrix X, Matrix Y, Matrix Z) {
    int k = 1;
    for (int n = max(max(X.height, X.width), Y.width) - 1; n > 0; n = n >> 1) {
        k = k << 1;
    }
    SubMatrix X1 = SubMatrix_Init(X, 0, 0, k, k);
    SubMatrix Y1 = SubMatrix_Init(Y, 0, 0, k, k);
    SubMatrix Z1 = SubMatrix_Init(Z, 0, 0, k, k);
    multShtrassenImp(X1, Y1, Z1);
}

void multShtrassenImp(SubMatrix X1, SubMatrix Y1, SubMatrix Z1) {
    int n = X1.height;
    if (n <= 16)
    {
        multNativeImp(X1, Y1, Z1);
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

    Matrix P1 = Matrix_Init(middle, middle);
    Matrix P2 = Matrix_Init(middle, middle);
    Matrix P3 = Matrix_Init(middle, middle);
    Matrix P4 = Matrix_Init(middle, middle);
    Matrix P5 = Matrix_Init(middle, middle);
    Matrix P6 = Matrix_Init(middle, middle);
    Matrix P7 = Matrix_Init(middle, middle);

    SubMatrix sP1 = SubMatrix_Init(P1, 0, 0, middle, middle);
    SubMatrix sP2 = SubMatrix_Init(P2, 0, 0, middle, middle);
    SubMatrix sP3 = SubMatrix_Init(P3, 0, 0, middle, middle);
    SubMatrix sP4 = SubMatrix_Init(P4, 0, 0, middle, middle);
    SubMatrix sP5 = SubMatrix_Init(P5, 0, 0, middle, middle);
    SubMatrix sP6 = SubMatrix_Init(P6, 0, 0, middle, middle);
    SubMatrix sP7 = SubMatrix_Init(P7, 0, 0, middle, middle);

    Matrix buff1 = Matrix_Init(middle, middle);
    Matrix buff2 = Matrix_Init(middle, middle);

    SubMatrix sbuff1 = SubMatrix_Init(buff1, 0, 0, middle, middle);
    SubMatrix sbuff2 = SubMatrix_Init(buff2, 0, 0, middle, middle);

    SubtractSubMatrix(F, H, sbuff1);
    multShtrassenImp(A, sbuff1, sP1);
    SumSubMatrix(A, B, sbuff1);
    multShtrassenImp(sbuff1, H, sP2);
    SumSubMatrix(C, D, sbuff1);
    multShtrassenImp(sbuff1, E, sP3);
    SubtractSubMatrix(G, E, sbuff1);
    multShtrassenImp(D, sbuff1, sP4);
    SumSubMatrix(A, D, sbuff1);
    SumSubMatrix(E, H, sbuff2);
    multShtrassenImp(sbuff1, sbuff2, sP5);
    SubtractSubMatrix(B, D, sbuff1);
    SumSubMatrix(G, H, sbuff2);
    multShtrassenImp(sbuff1, sbuff2, sP6);
    SubtractSubMatrix(A, C, sbuff1);
    SumSubMatrix(E, F, sbuff2);
    multShtrassenImp(sbuff1, sbuff2, sP7);


    SumSubMatrix(Z11, sP5, Z11);
    SumSubMatrix(Z11, sP4, Z11);
    SubtractSubMatrix(Z11, sP2, Z11);
    SumSubMatrix(Z11, sP6, Z11);

    SumSubMatrix(Z12, sP1, Z12);
    SumSubMatrix(Z12, sP2, Z12);

    SumSubMatrix(Z21, sP3, Z21);
    SumSubMatrix(Z21, sP4, Z21);

    SumSubMatrix(Z22, sP1, Z22);
    SumSubMatrix(Z22, sP5, Z22);
    SubtractSubMatrix(Z22, sP3, Z22);
    SubtractSubMatrix(Z22, sP7, Z22);

    Matrix_Free(P1);
    Matrix_Free(P2);
    Matrix_Free(P3);
    Matrix_Free(P4);
    Matrix_Free(P5);
    Matrix_Free(P6);
    Matrix_Free(P7);
    Matrix_Free(buff1);
    Matrix_Free(buff2);

}

#define MAX_VALUE 10


int main() {
    int n = 1024;
    Matrix A = Matrix_Init(n, n);
    Matrix B = Matrix_Init(n, n);
    Matrix C = Matrix_Init(n, n);
    unsigned int seed = 777; //time(NULL)
    srand(seed);

    printf("Native:\n");
    for (int i = 0; i < 10; i++) {
        Matrix_RandomFill(A, MAX_VALUE);
        Matrix_RandomFill(B, MAX_VALUE);

        Matrix_ZeroFill(C);

        double time_spent = 0.0;
        clock_t begin = clock();
        multNative(A, B, C);
        clock_t end = clock();
        printf("%d ms\n", (end - begin));
    }

    printf("Recurrent:\n");
    for (int i = 0; i < 10; i++) {
        Matrix_RandomFill(A, MAX_VALUE);
        Matrix_RandomFill(B, MAX_VALUE);

        Matrix_ZeroFill(C);

        double time_spent = 0.0;
        clock_t begin = clock();
        multRecurrent(A, B, C);
        clock_t end = clock();
        printf("%d ms\n", (end - begin));
    }

    printf("Shtrassen:\n");
    for (int i = 0; i < 10; i++) {
        Matrix_RandomFill(A, MAX_VALUE);
        Matrix_RandomFill(B, MAX_VALUE);

        Matrix_ZeroFill(C);

        double time_spent = 0.0;
        clock_t begin = clock();
        multShtrassen(A, B, C);
        clock_t end = clock();
        printf("%d ms\n", (end - begin));
    }

    //Matrix_RandomFill(A, MAX_VALUE);
    //Matrix_RandomFill(B, MAX_VALUE);

    //Matrix_ZeroFill(C);
    //multNative(A, B, C);
    //Matrix_Print(C);
    //printf("\n");
    //Matrix_ZeroFill(C);
    //multRecurrent(A, B, C);
    //Matrix_Print(C);
    //printf("\n");
    //Matrix_ZeroFill(C);
    //multShtrassen(A, B, C);
    //Matrix_Print(C);
    printf("\n");

    Matrix_Free(A);
    Matrix_Free(B);
    Matrix_Free(C);
}