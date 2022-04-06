#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Write 100 integers (in binary) to a file with the name given by the command-line
 * argument.  You can write them from 0 to 99 or use random to write random numbers.
 * This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    // apparently wb and w is equivalent in this c versiohn
    if ((fp = fopen(argv[1], "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    int error = 0;
    // We want to create a file with 100 integers.
    for (int i = 0; i < 100; i++) {
        int number = rand() % 100;
        
        error += fwrite(&number, sizeof(int), 1, fp);
    }

    if (error != 100) {
        fprintf(stderr, "Error: data not fully written to file\n");
    }




    fclose(fp);
    return 0;
}
