#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate space for 11 characters
    char phone[11];
    int i;
    int error = 0;

    scanf("%s", phone);
    while(scanf("%d", &i) != EOF) {
        if (0 <= i  && i <= 9) {
            printf("%c\n", phone[i]);
        }

        else if (i < -1 || i > 9) {
            error = 1;
            printf("ERROR\n");
        }

        else {
            printf("%s\n", phone);
        }
    }
    return error;
}