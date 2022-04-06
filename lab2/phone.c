#include <stdio.h>
#include <stdlib.h>

int main() {
    // Allocate space for 11 characters
    char phone[11];
    int i;

    // scan the input
    scanf("%s %d", phone, &i);
    if (0 <= i  && i <= 9) {
        printf("%c\n", phone[i]);
    }

    else if (i < -1 || i > 9) {
        printf("ERROR\n");
        return 1;
    }

    else {
        printf("%s\n", phone);
    }
    return 0;
}
