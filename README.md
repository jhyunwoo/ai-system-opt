# 행렬곱 계산 최적화

## 단일 프로세스 연산 프로그램 빌드
```
gcc -O2 main.c -o main    
```

```
./main
```

행렬 크기 지정 (3차원)
```
./main 2048 2048 2048
```

## 병렬 스레드 연산 프로그램 빌드

```
gcc -O2 multi_thread.c -o multi_thread    
```

행렬 크기 지정 (3차원), 스레드 수
```
./multi_thread 2048 2048 2048 4
```

## Project context and engineering approach

This is a systems-programming experiment that compares a straightforward matrix multiplication implementation with a multi-threaded variant. It makes the cost of large three-dimensional matrix operations observable and lets the caller control both dimensions and worker count.

The core problem is balancing parallel work against thread-management overhead and memory access behavior. The project keeps the implementations small and compilable with `gcc -O2`, so performance comparisons can focus on algorithm partitioning and CPU scheduling rather than framework overhead.

## Status

Study and benchmark code retained for performance-analysis exercises.
