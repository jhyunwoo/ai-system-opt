#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// A: (M x K), B: (K x N), C: (M x N)
void matmul(const double *A, const double *B, double *C, int M, int K, int N) {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int p = 0; p < K; p++) {
                sum += A[i * K + p] * B[p * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

// 행렬 출력 함수
static void print_matrix(const double *X, int R, int Cc) {
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < Cc; j++) {
            printf("%6.2f ", X[i * Cc + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    int M = 1024, K = 1024, N = 1024;
    if (argc == 4) {
        M = atoi(argv[1]);
        K = atoi(argv[2]);
        N = atoi(argv[3]);
    }

    size_t a_elems = (size_t)M * K;
    size_t b_elems = (size_t)K * N;
    size_t c_elems = (size_t)M * N;

    double *A = (double*)malloc(a_elems * sizeof(double));
    double *B = (double*)malloc(b_elems * sizeof(double));
    double *C = (double*)malloc(c_elems * sizeof(double));

    if (!A || !B || !C) {
        fprintf(stderr, "malloc 실패: 행렬이 너무 큽니다 (M=%d, K=%d, N=%d)\n", M, K, N);
        free(A); free(B); free(C);
        return 1;
    }

    for (size_t i = 0; i < a_elems; ++i) A[i] = (double)((i % 100) - 50) / 10.0;
    for (size_t i = 0; i < b_elems; ++i) B[i] = (double)(((i * 7) % 100) - 50) / 9.0;

    // 시간 측정
    double t0 = now_sec();
    matmul(A, B, C, M, K, N);
    double t1 = now_sec();

    // 체크섬
    double checksum = 0.0;
    for (size_t i = 0; i < c_elems; ++i) checksum += C[i];

    printf("행렬곱 계산 시간 = %.6f sec, checksum = %.6f (M=%d, K=%d, N=%d)\n",
           (t1 - t0), checksum, M, K, N);

    free(A);
    free(B);
    free(C);

    return 0;
}