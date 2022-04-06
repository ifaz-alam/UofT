#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"


/* Read the words from a filename and return a linked list of the words.
 *   - The newline character at the end of the line must be removed from
 *     the word stored in the node.
 *   - You an assume you are working with Linux line endings ("\n").  You are
 *     welcome to also handle Window line endings ("\r\n"), but you are not
 *     required to, and we will test your code on files with Linux line endings.
 *   - The time complexity of adding one word to this list must be O(1)
 *     which means the linked list will have the words in reverse order
 *     compared to the order of the words in the file.
 *   - Do proper error checking of fopen, fclose, fgets
 */

// Define a method that allows for a creation of a new node
struct node *create_node(char *value, struct node *next) {
    // reserve space in memory for our new node
    struct node *new_node = malloc(sizeof(struct node));
    
    // update the word attribute for our new node
    strncpy(new_node->word, value, SIZE);

    // make the new_node point to the node named next
    new_node->next = next;

    return new_node;
}

struct node *read_list(char *filename) {
    // We need to parse the file. Begin by opening the file in read mode
    FILE *words_file;
    words_file = fopen(filename, "r");

    // Handle errors if they arise
    if (words_file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    // Now we shall begin constructing our Linked List.
    // The idea here is to initialize NULL as the initial head pointer
    // Then for each word that we parse in our file, we set that word
    // To become the new head of our linked list until we have exhausted the entire file
    struct node *head = NULL;
    
    // variable for storing current word
    char word[SIZE];

    // parse the .txt file
    while (fscanf(words_file, "%s", word) == 1) {
        //printf("The current word is %s \n", word);
        head = create_node(word, head);
    }
    fclose(words_file);
    return head;
}


/* Print the words in the linked-list list one per line
 */
void print_dictionary(struct node *list) {
    
    struct node *curr = list;

    while (curr != NULL) {
        // print the word stored in the current node
        printf("%s", curr->word);
        printf("\n");
        curr = curr->next;
    }
}
/* Free all of the dynamically allocated memory in the dictionary list 
 */
void free_dictionary(struct node *list) {
    // We will free one node at a time, traversing and freeing
    struct node *placeholder;

    while (list != NULL) {
        placeholder = list;
        list = list->next;
        free(placeholder);
    }    
}