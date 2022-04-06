#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "constraints.h"

/* Read the wordle grid and solution from fp.
 * Return a pointer to a wordle struct.
 * See sample files for the format. Assume the input file has the correct
 * format.  In other words, the word on each is the correct length, the
 * words are lower-case letters, and the line ending is either '\n' (Linux,
 * Mac, WSL) or '\r\n' (Windows)
 */
struct wordle *create_wordle(FILE *fp)
{
    struct wordle *w = malloc(sizeof(struct wordle));
    char line[MAXLINE];
    w->num_rows = 0;

    while (fgets(line, MAXLINE, fp) != NULL)
    {

        // remove the newline character(s)
        char *ptr;
        if (((ptr = strchr(line, '\r')) != NULL) ||
            ((ptr = strchr(line, '\n')) != NULL))
        {
            *ptr = '\0';
        }

        strncpy(w->grid[w->num_rows], line, SIZE);
        w->grid[w->num_rows][SIZE - 1] = '\0';
        w->num_rows++;
    }
    return w;
}

/* Create a solver_node and return it.
 * If con is not NULL, copy con into dynamically allocated space in the struct
 * If con is NULL set the new solver_node con field to NULL.
 * Tip: struct assignment makes copying con a one-line statements
 */
struct solver_node *create_solver_node(struct constraints *con, char *word)
{   
    // reserve space in memory for our new solver_node
    struct solver_node *new_solver_node = malloc(sizeof(struct solver_node));

    // initially the sibling and child list pointers are NULL
    new_solver_node->next_sibling = NULL;
    new_solver_node->child_list = NULL;

    // copy the word onto the node and null terminate it.
    strncpy(new_solver_node->word, word, SIZE - 1);
    new_solver_node->word[SIZE - 1] = '\0';

    // set the new solver_node con field to NULL as per the given function specification
    if (con == NULL)
    {
        new_solver_node->con = NULL;
        return new_solver_node;
    }

    else
    {
        // reserve space into memory
        new_solver_node->con = init_constraints();

        // copy cannot be constraints from the given con
        for (int i = 0; i < ALPHABET_SIZE; i++)
        {
            new_solver_node->con->cannot_be[i] = con->cannot_be[i];
        }
    }

    return new_solver_node;
}

/*
Check if the given word contains duplicate letters found in the solution to wordle w
Return 1 if this is true, else return 0;
*/
int check_duplicates(char *word, struct wordle *w) {
    // we can pivot on each character in the solution word and count the number of occurences for that character
    // in the guess word. if the number of occurences of the character exceeds 1, then this means
    // we have a duplicate letter in the guess
    // notice that this holds because of the assumption in the assignment stating
    // the solution word will not contain any duplicate letters
    for (int i = 0; i < WORDLEN; i++)
    {
        // fix a pivot on our solution word
        char pivot = w->grid[0][i];

        // count the number of occurences of this pivot in our guess word
        int pivot_count = 0;
        for (int j = 0; j < WORDLEN; j++)
        {
            if (word[j] == pivot)
            {
                pivot_count += 1;
            }

            // we have duplicate so this is not a valid solution word
            if (pivot_count > 1)
            {
                return 1;
            }
        }
    }
    // no duplicates were found!
    return 0;
}

/* Return 1 if "word" matches the constraints in "con" for the wordle "w".
 * Return 0 if it does not match
 */
int match_constraints(char *word, struct constraints *con,
                      struct wordle *w, int row)
{   

    // we must check the following:
    // 1) check if the word[row] is in the must_be[row] string.
    // if it isn't then this word doesn't match the constraints.

    // 2) Also, if must_be[row] is the empty string and word[row] is in the cannot_be set
    // then this word does not match the constraints

    // 3) If there are multiple yellow tiles in this row, ensure that we're
    // not matching on words that contain duplicates of letters in the solution.

    // Before doing any of the above,
    // check if word has any duplicate characters that exist in the solution word
    // since the solution cannot contain duplicates at all.
    if (check_duplicates(word, w) == 1) {
            return 0;
        }

    // handle 1):
    // check if word[index] is in the must_be[index] string
    int num_yellow = 0;

    for (int i = 0; i < WORDLEN; i++)
    {   
        // set the contains_yellow flag to true, this will be used to evaluate case 3.
        if (w->grid[row][i] == 'y') {
            num_yellow = 1;
        }
        // as per piazza suggestions,
        // we can eliminate words where a yellow tile has a letter in the same
        // position as the solution word.
        if (w->grid[row][i] == 'y' && word[i] == w->grid[0][i])
        {
            return 0;
        }

        // This variable is used as a flag, to keep track of whether or not the i'th character in word is inside must_be
        int in_must_be = 0;
        for (int must_be_index = 0; must_be_index < strlen(con->must_be[i]); must_be_index++)
        {
            if (word[i] == con->must_be[i][must_be_index])
            {
                in_must_be = 1;
            }
        }
        // The i'th character word in word is not inside must_be, so it doesn't match the constraints.
        if (in_must_be == 0 && strlen(con->must_be[i]) != 0)
        {
            return 0;
        }

        // It is possible that the for loop above did not execute
        // This occurs in the case that must_be[i] is empty.
        // Just in case, we add an additional check.
        // In particular,
        // must_be[i] being the empty string and word[index] being in the cannot_be set
        // means that the word does not match the constraints
        // this handles 2)
        if (strlen(con->must_be[i]) == 0 && con->cannot_be[word[i] - 'a'] == 1)
        {
            return 0;
        }
    }

    // handle 3
    // we have multiple yellow tiles in this row
    // check if the word contains duplicates of letters in the solution
    if (num_yellow >= 2)
    {
        if (check_duplicates(word, w) == 1) {
            return 0;
        }
    }

    // if we have reached here this means that 0 was not returned.
    // in other words, the word matches the constraints!
    return 1;
}

/* remove "letter" from "word"
 * "word" remains the same if "letter" is not in "word"
 */
void remove_char(char *word, char letter)
{
    char *ptr = strchr(word, letter);
    if (ptr != NULL)
    {
        *ptr = word[strlen(word) - 1];
        word[strlen(word) - 1] = '\0';
    }
}

void construct_constraints(int row, struct wordle *w, struct solver_node *parent, char *word)
{
    // our parent is the root node
    // alternatively we could have used row == 1 as a condition but this is equivalent.
    if (row - 1 == 0)
    {
        // first we set yellow
        for (int index = 0; index < strlen(w->grid[row]); index++)
        {
            // next tile is implicitly 'ggggg'
            if (w->grid[row][index] == 'y')
            {   
                set_yellow(index, w->grid[row], "ggggg", word, parent->con);
                add_to_cannot_be(&word[index], parent->con);
            }
        }

        // then we use set_green to overwrite anything on must_be that was changed by set_yellow
        // we also update cannot_be appropriately.
        for (int index = 0; index < strlen(w->grid[row]); index++)
        {
            if (w->grid[row][index] == 'g')
            {
                set_green(word[index], index, parent->con);
            }

            if (w->grid[row][index] == '-')
            {
                // since its grey and the next word is the solution so it has all g's
                add_to_cannot_be(&word[index], parent->con);
            }
        }
        return;
    }

    // else
    // Here row < 1
    // From here we follow the specifications as per the slide instructions.

    for (int index = 0; index < strlen(w->grid[row]); index++)
    {   
        // the specifications for set_yellow:
        // set_yellow(int index, char *cur_tiles, char *next_tiles, char *word, struct constraints *con);
        if (w->grid[row][index] == 'y') {   
            set_yellow(index, w->grid[row], w->grid[row-1], word, parent->con);
        }

        // if the character on the next tile is green or - and the one on current tiles is yellow
        // we must add it to cannot_be
        // because if it was can_be it would have been green which it's not.
        if (w->grid[row][index] == 'y' && (w->grid[row - 1][index] == 'g' || w->grid[row - 1][index] == '-'))
        {   
            add_to_cannot_be(&word[index], parent->con);
        }
    }

    // then we use set_green to overwrite anything on must_be that was changed by set_yellow
    // we also update cannot_be appropriately.
    for (int index = 0; index < strlen(w->grid[row]); index++)
    {
        // the specifications for set_green:
        // set_green(char letter, int index, struct constraints *con)
        if (w->grid[row][index] == 'g')
        {
            set_green(word[index], index, parent->con);
        }

        if (w->grid[row][index] == '-' && w->grid[row - 1][index] == 'g')
        {
            // since its grey and the next tile is green
            add_to_cannot_be(&word[index], parent->con);
        }
 
        if (w->grid[row][index] == '-')
        {
            // since its grey
            add_to_cannot_be(&word[index], parent->con);
        }
    }
    return;
}

/* Build a tree starting at "row" in the wordle "w".
 * Use the "parent" constraints to set up the constraints for this node
 * of the tree
 * For each word in "dict",
 *    - if a word matches the constraints, then
 *        - create a copy of the constraints for the child node and update
 *          the constraints with the new information.
 *        - add the word to the child_list of the current solver node
 *        - call solve_subtree on newly created subtree
 */

void solve_subtree(int row, struct wordle *w, struct node *dict,
                   struct solver_node *parent)
{
    // the base case of the recursion is
    // when we reach the end of the rows in the wordle input w.
    if (row == w->num_rows)
    {   
        // This is simply for showing the constraints in our recursion base case.
        // There is a similar call to print_constraints in our recursive case.
        if (verbose)
        {
            printf("Running solve_subtree: %d, %s\n", row, parent->word);
            print_constraints(parent->con);
        }
        return;
    }
    // when calling this function, we have a reference to the parent node
    if (row < w->num_rows)
    {
        // this is the first call to the function
        // the first row of the wordle grid is our solution word, so we assign it to the parent word attribute
        if (row == 1)
        {
            strncpy(parent->word, w->grid[0], SIZE - 1);
            // null terminate the string
            parent->word[SIZE - 1] = '\0';
            // because initially te parent constraint is NULL when we first call solve_subtree
            construct_constraints(row, w, parent, w->grid[0]);
        }

        // this is not the first call to the function
        // so the parent is a reference to some non-root node in the tree
        // when a parent has no child, we construct the constraints for the parent node
        // then check if any of the words in our dict linked list matches the constraints
        else if (row > 1 && parent->child_list == NULL) {
            construct_constraints(row, w, parent, parent->word);
        }
        
        // The parent node now has constraints that each of its children should follow.
        // we now traverse the words in the dictionary linked list and call match_constraints
        struct node *curr = dict;
        while (curr != NULL)
        {
            // if a word matches, then we create a new struct_solver_node, add it to the list of children of this node,
            // and call solve_subtree on it
            if (match_constraints(curr->word, parent->con, w, row))
            {   
                struct solver_node *new_node = create_solver_node(parent->con, curr->word);

                // let this new_node be the head of the child_list for parent.
                new_node->next_sibling = parent->child_list;
                parent->child_list = new_node;
                // update the word attribute for new_node and null terminate it
                // call solve subtree on new_node, and go down a row in our subtree
                solve_subtree(row + 1, w, dict, new_node);
            }
            curr = curr->next;
        }
    }

    // Show the constraints in the recursive case when calling verbose mode.
    if (verbose)
    {
        printf("Running solve_subtree: %d, %s\n", row, parent->word);
        print_constraints(parent->con);
    }
}

/* Print to standard output all paths that are num_rows in length.
 * - node is the current node for processing
 * - path is used to hold the words on the path while traversing the tree.
 * - level is the current length of the path so far.
 * - num_rows is the full length of the paths to print
 */
void print_paths(struct solver_node *node, char **path,
                 int level, int num_rows)
{
    // our base case is when level == num_rows,
    // signalling that we are done traversing down a certain path
    if (level == num_rows)
    {   
        // add the final word to our path
        path[level - 1] = node->word;

        // since path is an array of strings, and we know level is the current length of the path so far
        // we can iterate over this array and print the contents accordingly
        for (int i = 0; i < num_rows; i++)
        {
            printf("%s ", path[i]);
        }
        printf("\n");
    }

    else
    {
        // The way as to how I am thinking about traversing the tree is as follows:
        // For each child that belongs to the current node, if that child is not NULL and it has children
        // Then we can construct a path that travels through that child.
        // Essentially this suggests that we can recurse on each child to aggregate each unique path.
        // Totalling all these recursive combinations leads to all possible paths as desired

        // This stores the most recent word found on the path
        path[level - 1] = node->word;

        // We initialize a reference to the first child of node
        struct solver_node *curr_child = node->child_list;
        
        while (curr_child != NULL)
        {
            // we can go down a level in our path, so we recurse onto that child
            print_paths(curr_child, path, level + 1, num_rows);
            
            // go to the sibling of the child
            curr_child = curr_child->next_sibling;
        }
    }
}

/* Free all dynamically allocated memory pointed to from w.
 */
void free_wordle(struct wordle *w)
{
    free(w);
}

/* Free all dynamically allocated pointed to from node
 */
void free_tree(struct solver_node *node)
{   
    // we don't need to free anything for a NULL node
    if (node == NULL) {
        return;
    }

    else {
        // Here node is not NULL
        
        // Store a reference to node
        struct solver_node *curr = node;
        while (curr != NULL)
        {   
            // recurse onto child node
            free_tree(curr->child_list);

            // We can now free the current node as we wont ever revisit again because our recursion
            // Goes downwards
            // Recall inside a node, we have a constraint that was malloc'ed so we need to free that first.  
            // After that, we can free the node itself.  
            free_constraints(curr->con);
            free(curr);
            curr = curr->next_sibling;
        }
    }
}