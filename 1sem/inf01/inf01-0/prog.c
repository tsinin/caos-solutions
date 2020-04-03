#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int count, const char** z_str) {
    float x;
    int y;
    char* endPtr;
    int z = strtoll(z_str[1], &endPtr, 27);
    scanf("%f", &x);
    scanf("%x", &y);
    float ans = x + (float)y + (float)z;
    printf("%.3f", ans);
    return 0;
}
