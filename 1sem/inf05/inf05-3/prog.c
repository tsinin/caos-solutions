#include <immintrin.h>
#include <smmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>

int main()
{
    int M, N;
    scanf("%d %d", &M, &N);

    float* A = aligned_alloc(32 * 8, sizeof(int) * M * N);
    float* B = aligned_alloc(32 * 8, sizeof(int) * M * N);

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            scanf("%f", A + i * N + j);
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            scanf("%f", B + j * N + i);
        }
    }

    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            // print ans[i][j] = sum(A[i][k] * B[k][j], 0 <= k < N)
            // A[i][k] = A[i * N + k], B[k][j] = B[j * N + k]
            float ans_elmnt = 0;
            int k = 0;
            while (k + 8 <= N) {
                __m256 reg1, reg2;
                reg1 = _mm256_loadu_ps(A + i * N + k);
                reg2 = _mm256_loadu_ps(B + j * N + k);
                reg1 = _mm256_mul_ps(reg1, reg2);
                reg1 = _mm256_hadd_ps(reg1, reg1);
                reg1 = _mm256_hadd_ps(reg1, reg1);
                __m128 first = _mm256_extractf128_ps(reg1, 0);
                __m128 second = _mm256_extractf128_ps(reg1, 1);
                __m128 sum = _mm_add_ps(first, second);
                ans_elmnt += _mm_cvtss_f32(sum);
                k += 8;
            }
            while (k + 4 <= N) {
                __m128 reg1, reg2;
                reg1 = _mm_loadu_ps(A + i * N + k);
                reg2 = _mm_loadu_ps(B + j * N + k);
                reg1 = _mm_dp_ps(reg1, reg2, 255);
                ans_elmnt += _mm_cvtss_f32(reg1);
                k += 4;
            }
            while (k < N) {
                ans_elmnt += A[i * N + k] * B[j * N + k];
                ++k;
            }

            printf("%f ", ans_elmnt);
        }
        printf("\n");
    }
    return 0;
}
