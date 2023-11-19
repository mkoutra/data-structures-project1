#include <stdio.h>

int main(void) {
    for (int i = 0; i < 6; ++i) {
        if (i == 3) continue;
        printf("%d\n", i);
    }
    return 0;
}