#include <stdlib.h>
#include <stdio.h>

/*
 * Define a function void fib(...) below. This function takes parameter n
 * and generates the first n values in the Fibonacci sequence.  Recall that this
 * sequence is defined as:
 *         0, 1, 1, 2, 3, 5, ... , fib[n] = fib[n-2] + fib[n-1], ...
 * The values should be stored in a dynamically-allocated array composed of
 * exactly the correct number of integers. The values should be returned
 * through a pointer parameter passed in as the first argument.
 *
 * See the main function for an example call to fib.
 * Pay attention to the expected type of fib's parameters.
 */

/* Write your solution here */
// count refers to how many terms in the fibonacci sequence we wish to see
void fib(int **fib_sequence, int count) {
    // recall that 
    // f(n) = 0 if n = 0
    // f(n) = 1 if n = 1
    // f(n) = f(n-1) + f(n-2) if n > 1
    // here count represents n
    if (count == 1) {
        (*fib_sequence)[0] = 0;
    }

    else if (count == 2) {
        (*fib_sequence)[0] = 0;
        (*fib_sequence)[1] = 1;
    }
    
    else {
        (*fib_sequence)[0] = 0;
        (*fib_sequence)[1] = 1;
        for (int i = 2; i < count; i++) {
            (*fib_sequence)[i] = (*fib_sequence)[i - 1] + (*fib_sequence)[i - 2];
        }
    }
    
}

int main(int argc, char **argv) {
    /* do not change this main function */
    int count = strtol(argv[1], NULL, 10);

    // fib_sequence holds pointers to integer values
    int *fib_sequence;

    // we need space for count integer pointers
    fib_sequence = malloc(sizeof(int) * count);
    
    fib(&fib_sequence, count);
    
    
    for (int i = 0; i < count; i++) {
        printf("%d ", fib_sequence[i]);
    }
    
    
    
    free(fib_sequence);

    return 0;
}
