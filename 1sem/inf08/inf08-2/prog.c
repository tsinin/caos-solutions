#include <fcntl.h>
#include <inttypes.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int N;

int get_number(int i, int j)
{
    int x;
    if (i <= j && i <= N - 1 - i && i <= N - 1 - j)
        x = i;
    if (j <= i && j <= N - 1 - i && j <= N - 1 - j)
        x = j;
    if (N - 1 - i <= i && N - 1 - i <= j && N - 1 - i <= N - 1 - j)
        x = N - 1 - i;
    if (N - 1 - j <= i && N - 1 - j <= j && N - 1 - j <= N - 1 - i)
        x = N - 1 - j;
    if (i <= j) {
        return 1 + 4 * x * (N - x) + (i - x) + (j - x);
    } else {
        return 4 * (x + 1) * (N - x - 1) - (i - x) - (j - 1 - x);
    }
}

int get_len(int num)
{
    int ans = 0;
    while (num) {
        ans++;
        num /= 10;
    }
    return ans;
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        return 1;
    }
    char* file_name = argv[1];
    N = atoi(argv[2]);
    int W = atoi(argv[3]);
    int file_d = open(
        file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    if (file_d == -1) {
        return 1;
    }
    lseek(file_d, (long long)N * N * W + N - 1, SEEK_SET);
    write(file_d, "", 1);
    char* mem_ptr = mmap(
        NULL,
        (long long)N * N * W + N,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        file_d,
        0);
    if (mem_ptr == MAP_FAILED) {
        return 1;
    }

    char tmp[W + 1];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int number = get_number(i, j);
            int len = get_len(number);

            memset(tmp, ' ', W - len);
            tmp[W - len] = '\0';
            sprintf(tmp + W - len, "%d", number);
            memcpy(mem_ptr + (N * W + 1) * i + j * W, tmp, W);
        }
        memcpy(mem_ptr + (N * W + 1) * i + N * W, "\n", 1);
    }

    msync(mem_ptr, (long long)N * N * W + N, MS_SYNC);
    munmap(mem_ptr, (long long)N * N * W + N);
    close(file_d);
    return 0;
}
