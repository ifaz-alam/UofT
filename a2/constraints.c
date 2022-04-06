#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "constraints.h"

/* Create and initialize a constraints struct.
 * Sets the fields to 0 or empty string.
 * Return a pointer to the newly created constraints struct.
 */
struct constraints *init_constraints() {
    struct constraints *struct_to_return = malloc(sizeof(struct constraints));

    // Set the fields to 0 of cannot_be to 0
    for (int i=0; i<ALPHABET_SIZE; i++){
        struct_to_return->cannot_be[i] = 0;
    }

    // Set the fields of must_be to empty string
    for (int j = 0; j < WORDLEN; j++){
        for (int k = 0; k < SIZE; k++){
            struct_to_return->must_be[j][k] = '\0';
        }
    }

    // Return a pointer to the newly created constraints struct.
    return struct_to_return;
}

/* Update the "must_be" field at "index" to be a string
 * containing "letter"
 * The tile at this index is green, therefore the letter at "index"
 * must be "letter"
 */
void set_green(char letter, int index, struct constraints *con) {
    assert(islower(letter));
    assert(index >= 0 && index < WORDLEN);

    // Initialize an array that we can concatenate to the must_be[index] field
    char a_letter[2] = {letter, '\0'};
    strncpy(con->must_be[index], a_letter, SIZE - 1);

    // Ensure the must_be[index] field is null terminated
    con->must_be[index][SIZE - 1] = '\0';
}

/* Update "con" by adding the possible letters to the string at the must_be
 * field for "index".
 * - index is the index of the yellow tile in the current row to be updated
 * - cur_tiles is the tiles of this row
 * - next_tiles is the tiles of the row that is one closer to the solution row
 * - word is the word in the next row (assume word is all lower case letters)
 * Assume cur_tiles and next_tiles contain valid characters ('-', 'y', 'g')
 *
 * Add to the must_be list for this index the letters that are green in the
 * next_tiles, but not green in the cur_tiles or green or yellow in the
 * next_tiles at index.
 * Also add letters in yellow tiles in next_tiles.
 */
void set_yellow(int index, char *cur_tiles, char *next_tiles,
                char *word, struct constraints *con) {

    assert(index >=0 && index < WORDLEN);
    assert(strlen(cur_tiles) == WORDLEN);
    assert(strlen(next_tiles) == WORDLEN);
    assert(strlen(word) == WORDLEN);

    for (int i=0; i<WORDLEN; i++) {
        // as per the Piazza FAQ set_yellow logic suggestion,
        // we simply concatenate word[i] to must_be[index]
        if (i != index && (next_tiles[i] == 'g' || next_tiles[i] == 'y') && cur_tiles[i] != 'g' && word[i] != word[index]) {
            char a_letter[1] = {word[i]};
            strncat(con->must_be[index], a_letter, SIZE - strlen(con->must_be[index]) -1);
        }
        
    }
}

/* Add the letters from cur_word to the cannot_be field.
 * See the comments in constraints.h for how cannot_be is structured.
 */
void add_to_cannot_be(char *cur_word, struct constraints *con) {
    assert(strlen(cur_word) <= WORDLEN);

    // we iterate through each character in cur_word and as
    // per the assignment instructions, we use the fact that
    // ASCII values of characters are sequential to determine
    // the appropriate index of cannot_be that is to be set to 1
    for (int i=0; i<strlen(cur_word); i++) {
        int index = cur_word[i] - 'a';
        con->cannot_be[index] = 1;
    }
}

void print_constraints(struct constraints *c) {
    printf("cannot_be: ");

    // we iterate over the elements of cannot_be and 
    // display which characters are in cannot_be
    for (int i=0; i<ALPHABET_SIZE; i++){
        if (c->cannot_be[i] == 1) {
            printf("%c ", 'a' + i);
        }
    }

    printf("\nmust_be\n");
    // we display the characters that are in must_be
    for (int i=0; i<WORDLEN; i++){
        printf("[%d] ", i);

        for (int j=0; j< SIZE;j++){
            if (c->must_be[i][j] - 'a' >= 0  && c->must_be[i][j] - 'a' < ALPHABET_SIZE) {
                printf("%c ", c->must_be[i][j]);
            }
        }
        printf("\n");

    }
}

/* Free all dynamically allocated memory pointed to by c 
 */  
void free_constraints(struct constraints *c) {  
    free(c);
}  