#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
    // **zyx...cbaZYX...CBA987...210
    uint64_t answer = 0;
    uint64_t current = 0;
    char symbol;
    while (scanf("%c", &symbol) != EOF) {
        if (isdigit(symbol)) {
            current |= 1llu << (symbol - '0');
        } else if (isupper(symbol)) {
            current |= 1llu << (10 + symbol - 'A');
        } else if (islower(symbol)) {
            current |= 1llu << (10 + 26 + symbol - 'a');
        } else {
            if (symbol == '&') {
                answer &= current;
            }
            if (symbol == '|') {
                answer |= current;
            }
            if (symbol == '^') {
                answer ^= current;
            }
            if (symbol == '~') {
                answer = ~answer;
            }
            current = 0;
        }
    }
    for (int i = 0; i < 62; ++i) {
        if (answer & 1llu) {
            if (i < 10) {
                printf("%c", '0' + i);
            } else if (i < 36) {
                printf("%c", 'A' + i - 10);
            } else {
                printf("%c", 'a' + i - 36);
            }
        }
        answer >>= 1;
    }
    return 0;
}
