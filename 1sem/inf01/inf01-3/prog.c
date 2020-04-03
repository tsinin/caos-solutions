#include <stdio.h>

int main(int count, char **args) {
    for (int i = 1; i < count; ++i) {
        int ans = 0;
        for (int j = 2; args[i][j] != '\0'; ++j) {
            if (!ans && args[i][j] != '0')
                ans++;
            if (ans)
                ans++;
        }
        ans >>= 1;
        if (!ans)
            ans++;
        printf("%i ", ans);
    }
    return 0;
}
