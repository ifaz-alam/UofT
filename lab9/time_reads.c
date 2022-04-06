/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

void handler() {
    fprintf(stdout, MESSAGE, num_reads, seconds);
    exit(0);
}

unsigned int alarm (unsigned int seconds)
{
  struct itimerval old, new;
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = 0;
  new.it_value.tv_sec = (long int) seconds;
  if (setitimer (ITIMER_PROF, &new, &old) < 0)
    return 0;
  else
    return old.it_value.tv_sec;
}
/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */

    // 600 bytes, each integer takes 5 bytes, 600/5 = 120
    // generate from 1 to 120, multiply by 5 to get pos
    // random is from 0 to rand_max, so randomize from (0 to 119) add 1 then multiply by 5
    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0; // default flags
    sigemptyset(&newact.sa_mask); //  block no signals during handler
    sigaction(SIGPROF, &newact, NULL);
    alarm(seconds);
    //setitimer(ITIMER_PROF, &alarm, NULL);
    for (;;) {
        num_reads += 1;
        fseek(fp, (rand() % 100) * sizeof(int), SEEK_SET);
        int value;
        int error = fread(&value, sizeof(int), 1, fp);
        if (error == 1) {
            printf("%d\n", value);
        }
        else {
            fprintf(stderr, "ERROR: position could not be read");
        }



    }
    return 1; // something is wrong if we ever get here!
}
