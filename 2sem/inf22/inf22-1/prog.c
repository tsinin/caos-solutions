#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <unistd.h>

#include <openssl/evp.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Need 1 argument - password.\n");
        return 1;
    }
    char* password = argv[1];

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    const size_t buf_size = 1 << 20;
    char* buf = malloc(buf_size);
    memset(buf, '\0', buf_size);

    size_t need_to_read = 16;
    size_t readed;
    while (need_to_read) {
        readed = read(0, buf + strlen(buf), need_to_read);
        need_to_read -= readed;
    }

    unsigned char key[32];
    unsigned char iv[16];

    // Генерация ключа и начального вектора из
    // пароля произвольной длины и 8-байтной соли
    EVP_BytesToKey(
        EVP_aes_256_cbc(),    // алгоритм шифрования
        EVP_sha256(),         // алгоритм хеширования пароля
        buf + 8,              // соль
        password, strlen(password), // пароль
        1,                    // количество итераций хеширования
        key,          // результат: ключ нужной длины
        iv            // результат: начальный вектор нужной длины
    );

    // Начальная стадия: инициализация
    EVP_DecryptInit(
        ctx,                  // контекст для хранения состояния  
        EVP_aes_256_cbc(),    // алгоритм шифрования
        key,                  // ключ нужного размера
        iv                    // начальное значение нужного размера
    );

    const size_t ans_size = 1 << 25;
    char* ans = malloc(ans_size);

    int total_len = 0;
    int len;
    while ((readed = read(0, buf, buf_size)) > 0) {
        EVP_DecryptUpdate(ctx, ans + total_len, &len, (uint8_t *)buf, readed);
        total_len += len;
    }

    EVP_DecryptFinal_ex(ctx, ans + total_len, &len);
    total_len += len;

    size_t need_to_write = total_len;
    size_t written;
    len = 0;
    while (need_to_write) {
        written = write(1, ans + len, need_to_write);
        need_to_write -= written;
        len += written;
    }

    EVP_CIPHER_CTX_free(ctx);
    free(buf);
    free(ans);
    return 0;
}
