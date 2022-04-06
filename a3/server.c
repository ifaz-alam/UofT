#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <getopt.h>

// The maximum length of an HTTP message line
#define MAX_LINE 256
// The maximum length of an HTTP response message
#define MAX_LENGTH 16*1024
// The size of a chunk of HTTP response to read from the pipe
#define CHUNK_SIZE 1024


void printError(char *);
void printServerError();
void printResponse(char *str);

int debug = 0;

int main(int argc, char **argv) {
    //char msg[MAX_LENGTH];
    int result;
    FILE *fp = stdin; // default is to read from stdin

    // Parse command line options.
    int opt;
    while((opt = getopt(argc, argv, "v")) != -1) {
        switch(opt) {
            case 'v':
                debug = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-v] [filename]\n", argv[0]);
                _exit(1);
        }
    }
    if(optind < argc) {
        if((fp = fopen(argv[optind], "r")) == NULL) {
            perror("fopen");
            _exit(1);
        }
    }
    
    // variable that stores characters on each line that we read
    char line[MAX_LINE];
    
    while (fgets(line, MAX_LENGTH, fp) != NULL) {
        // search for GET requests
        if (strstr(line, "GET") != NULL) {
            int path_start_index, query_start_index = 0;
            int path_end_index, query_end_index = strlen(line) - 1;

            // flag variables
            int query_string_found = 0;
            int path_found = 0;

            // This block extracts the path variable
            for (int i = 0; i < strlen(line); i++) {
                if (line[i] == '/') {
                    path_start_index = i;
                    
                    // we search for the maximal index
                    for (int j = i; j < strlen(line); j++) {
                        if (line[j] == '?' || line[j] == ' ') {
                            path_end_index = j - 1;
                            break;
                        }
                    }

                    path_found = 1;
                    break;
                }
            }

            // This block extracts the query string variable
            for (int i = 0; i < strlen(line); i++) {
                if (line[i] == '?') {
                    // we don't want to have the ? in our string, so we start at the next character
                    query_start_index = i + 1;
                    
                    // we have found the end of the query string
                    // note we are assuming that the next chunk is the HTTP version
                    for (int j = i + 1; j < strlen(line); j++) {
                        if (line[j] == ' ') {
                            query_end_index = j - 1;
                            break;
                        }
                    }

                    query_string_found = 1;
                    break;
                }
            }

            if (query_string_found == 1) {
                // we construct the query string based on the indices we got
                char query_string[query_end_index - query_start_index + 2];
                strncpy(query_string, &line[query_start_index], sizeof(query_string));
                
                // null terminate query_string
                query_string[query_end_index - query_start_index + 1] = '\0';

                // set environment variable to the constructed query string
                setenv("QUERY_STRING", query_string, 1);
            }

            else {
                setenv("QUERY_STRING", "", 1);
            }
            
            // make space in memory for the length of the string + null termination character
            // end - start doesnt count end, so we had 1, then add 1 again for the null termination character
            // we found a valid path
            if (path_found == 1) { 
                // firstly we create a pipe
                // initialize a variable for the file descriptor
                int fd[2];

                //create a pipe and handle error if it arises
                if (pipe(fd) == -1) {
                    perror("pipe");
                }

                // make space in memory for the length of the string + null termination character
                // end - start doesnt count end, so we had 1, then add 1 again for the null termination character
                char path[path_end_index - path_start_index + 2];
                strncpy(path, &line[path_start_index], sizeof(path));

                // null terminate path
                path[path_end_index - path_start_index + 1] = '\0';
                
                // a child process that has a 404 error exits with code 2 
                int EXIT_CODE_404_ERROR = 2;

                // create a child process that will execute the cgi program
                result = fork();

                // parent process
                if (result > 0) {
                    // this will read from pipe, and not write
                    // so we close writing end of parents file descriptor
                    close(fd[1]);

                    int status;
                    wait(&status);

                    // Read from the pipe if child terminated normally
                    if (WIFEXITED(status)) {
                        // Check if it exited with a status of unable to read file.
                        // Otherwise the file was read successfully and we can extract its contents
                        if (WEXITSTATUS(status) == EXIT_CODE_404_ERROR) {
                            // child failed to open cgi program, hence 404 error
                            printError(&path[1]);
                        }
                        
                        else {
                            char buffer[CHUNK_SIZE];
                            buffer[CHUNK_SIZE - 1] = '\0';
                            buffer[0] = '\0';

                            char http_response[MAX_LENGTH + 1];
                            http_response[MAX_LENGTH] = '\0';

                            // read data in fixed size chunks and append them together
                            int num_read = 0;
                            while ((num_read = read(fd[0], buffer, CHUNK_SIZE)) > 0) {
                                strncat(http_response, buffer, num_read);
                                buffer[0] = '\0';
                            }
                            // show 200 OK
                            printResponse(http_response);
                            // reset the response buffer
                            http_response[0] = '\0';
                        }
                    }
                    
                    // child terminated abnormally, hence 500 error
                    else if (WIFSIGNALED(status)) {
                       printServerError();
                    }
                    // close file descriptors we are done using
                    close(fd[0]);
                }

                // child process
                else if (result == 0) {
                    // child does work here
                    // We need to construct the executable string by adding the . in front
                    char executable_string[1 + strlen(path) + 1];
                    executable_string[0] = '.';
                    executable_string[1] = '\0';
                    strncat(executable_string, path, sizeof(executable_string) - strlen(executable_string) - 1);

                    // fp stream is not being used in child, so we close it
                    fclose(fp);
                    
                    // reset stdout so it writes to the pipe
                    if (dup2(fd[1], fileno(stdout)) == -1) {
                        perror("dup2");
                        _exit(1);
                    }

                    // close reading end because we are writing to parent
                    close(fd[0]);
                    
                    // Attempt executing the file, but if it cannot execute we must exit
                    if (execl(executable_string, &executable_string[2], NULL) == -1) {
                        _exit(EXIT_CODE_404_ERROR);
                    }

                    // if we got here, then the child executed the program successfully
                    // we close file descriptors we dont need and exit gracefully
                    close(fd[1]);
                    _exit(0);
                }
                
                // unsuccessful fork
                else {
                    perror("fork");
                    _exit(1);
                }
            }
        }
    }

    if(fp != stdin) {
        if(fclose(fp) == EOF) {
            perror("fclose");
            _exit(1);
        }
    }
}

/* Print an http error page  
 * Arguments:
 *    - str is the path to the resource. It does not include the question mark
 * or the query string.
 */
void printError(char *str) {
    printf("HTTP/1.1 404 Not Found\r\n\r\n");

    printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
    printf("<html><head>\n");
    printf("<title>404 Not Found</title>\n");
    printf("</head><body>\n");
    printf("<h1>Not Found</h1>\n");
    printf("The requested resource %s was not found on this server.\n", str);
    printf("<hr>\n</body></html>\n");
}

/* Prints an HTTP 500 error page 
 */
void printServerError() {
    printf("HTTP/1.1 500 Internal Server Error\r\n\r\n");

    printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
    printf("<html><head>\n");
    printf("<title>500 Internal Server Error</title>\n");
    printf("</head><body>\n");
    printf("<h1>Internal Server Error</h1>\n");
    printf("The server encountered an internal error or\n");
    printf("misconfiguration and was unable to complete your request.<p>\n");
    printf("</body></html>\n");
}


/* Prints a successful response message
 * Arguments:
 *    - str is the output of the CGI program
 */
void printResponse(char *str) {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("%s", str);
}
char http_response[MAX_LENGTH + 1];
