#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 0;
    }
    HANDLE input = CreateFileA(
            argv[1], GENERIC_READ, 0, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (input == INVALID_HANDLE_VALUE) {
        return 1;
    }
    int value = 0;
    DWORD next_pointer = 0;
    DWORD ptr = 1;
    DWORD numberOfBytesRead;
    while (ptr != 0) {
        ReadFile(input, &value, sizeof(value), &numberOfBytesRead, NULL);
        if(numberOfBytesRead == 0) {
            return 0;
        }
        ReadFile(input, &next_pointer, sizeof(next_pointer), NULL, NULL);
        printf("%d ", value);
        ptr = SetFilePointer(input, next_pointer, NULL, FILE_BEGIN);
    }
    return 0;
}
