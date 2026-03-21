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