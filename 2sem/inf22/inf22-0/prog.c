#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <unistd.h>

#include <openssl/sha.h>

int main() {
    SHA512_CTX ctx;
    const size_t buf_size = 1 << 20;
    char* buf = malloc(buf_size);
    uint8_t out[SHA512_DIGEST_LENGTH];

    SHA512_Init(&ctx);

    size_t readed;
    while ((readed = read(0, buf, buf_size)) > 0) {
        SHA512_Update(&ctx, (uint8_t *)buf, readed);
    }

    SHA512_Final(out, &ctx);

    printf("0x");
    size_t i = 0;
    while (out[i] == 0) {
        ++i;
    }
    for (; i < SHA512_DIGEST_LENGTH; i++)
        printf("%02x", out[i]);
    printf("\n");
    free(buf);
    return 0;
}
