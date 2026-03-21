#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

static inline double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

typedef struct {
    const double *A;
    const double *B;
    double *C;
    int M, K, N;
    int row_start;   // [row_start, row_end) 구간 담당
    int row_end;
} Task;

static void* worker(void *arg) {
    Task *t = (Task*)arg;
    const double *A = t->A, *B = t->B;
    double *C = t->C;
    int M = t->M, K = t->K, N = t->N;

    for (int i = t->row_start; i < t->row_end; ++i) {
        size_t iK = (size_t)i * K;
        size_t iN = (size_t)i * N;
        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int p = 0; p < K; ++p) {
                sum += A[iK + p] * B[(size_t)p * N + j];
            }
            C[iN + j] = sum;
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    int M = 1024, K = 1024, N = 1024;
    int T = -1; // 스레드 수(미지정 시 코어 수)
    if (argc >= 4) {
        M = atoi(argv[1]);
        K = atoi(argv[2]);
        N = atoi(argv[3]);
    }
    if (argc >= 5) {
        T = atoi(argv[4]);
    }

    // 기본 스레드 수: 온라인 코어 수 또는 4
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (T <= 0) T = (cores > 0) ? (int)cores : 4;
    if (T > M) T = M; // 행 수보다 스레드가 많을 필요는 없음

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

    // 간단 초기화
    for (size_t i = 0; i < a_elems; ++i) A[i] = (double)((i % 100) - 50) / 10.0;
    for (size_t i = 0; i < b_elems; ++i) B[i] = (double)(((i * 7) % 100) - 50) / 9.0;

    // 스레드 생성 준비
    pthread_t *th = (pthread_t*)malloc(sizeof(pthread_t) * (size_t)T);
    Task *tasks = (Task*)malloc(sizeof(Task) * (size_t)T);
    if (!th || !tasks) {
        fprintf(stderr, "스레드 리소스 할당 실패\n");
        free(th); free(tasks);
        free(A); free(B); free(C);
        return 1;
    }

    int rows_per_thread = (M + T - 1) / T; // ceil(M/T)

    double t0 = now_sec();
    for (int t = 0; t < T; ++t) {
        int rs = t * rows_per_thread;
        int re = rs + rows_per_thread;
        if (re > M) re = M;

        tasks[t] = (Task){
            .A = A, .B = B, .C = C,
            .M = M, .K = K, .N = N,
            .row_start = rs, .row_end = re
        };
        pthread_create(&th[t], NULL, worker, &tasks[t]);
    }
    for (int t = 0; t < T; ++t) {
        pthread_join(th[t], NULL);
    }
    double t1 = now_sec();

    // 체크섬
    double checksum = 0.0;
    for (size_t i = 0; i < c_elems; ++i) checksum += C[i];

    printf("스레드=%d, 행렬곱 계산 시간 = %.6f sec, checksum = %.6f (M=%d, K=%d, N=%d)\n",
           T, (t1 - t0), checksum, M, K, N);

    free(th);
    free(tasks);
    free(A);
    free(B);
    free(C);
    return 0;
}