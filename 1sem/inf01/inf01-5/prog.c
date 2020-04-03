#include <stdint.h>
#include <stdio.h>

int main()
{
    typedef union {
        char real_char;
        uint8_t uint_value;
    } real_or_uint;
    real_or_uint value;
    int expecting = 0, ascii = 0, not_ascii = 0;
    while (scanf("%c", &value.real_char) != EOF) {
        if ((expecting && (value.uint_value >> 6) != 2) ||
            (!expecting && (value.uint_value >> 6) == 2)) {
            printf("%d %d\n", ascii, not_ascii);
            return 1;
        } else if (expecting) {
            expecting--;
            if (!expecting) {
                not_ascii++;
            }
        } else if (!(value.uint_value >> 7)) {
            ascii++;
        } else if (value.uint_value < 0xDF) {
            expecting = 1;
        } else if (value.uint_value < 0xEF) {
            expecting = 2;
        } else {
            expecting = 3;
        }
    }
    printf("%d %d\n", ascii, not_ascii);
    return 0;
}
