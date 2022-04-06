#include <stdio.h>  
#include <stdlib.h>  
  
/* Return a pointer to an array of two dynamically allocated arrays of ints. 
   The first array contains the elements of the input array s that are 
   at even indices.  The second array contains the elements of the input 
   array s that are at odd indices. 
 
   Do not allocate any more memory than necessary. You are not permitted 
   to include math.h.  You can do the math with modulo arithmetic and integer 
   division. 
*/  
int **split_array(const int *s, int length) {  
  int even = 0;  
  int odd = 0;  
  
  if (length % 2 == 0) {  
    even = length / 2;  
    odd = length / 2;  
  }  
  
  // if a list is of odd length, its last index will be even.   
  // so length - 1 is a list of even length and contains exactly (length - 1) / 2 even. + 1 for the last index   
    
  else {  
    odd = (length - 1) / 2;  
    even = ((length - 1) / 2) + 1;

    //even = (length + 1) / 2;
    //odd = length / 2;
  }  
  // we want to allocate space in memory to hold even ints and odd ints  
  // an array of integers is of type int*, an array is a pointer to the first element  
  int *even_arr = malloc(sizeof(int) * even);  
  int *odd_arr = malloc(sizeof(int) * odd);  
  
  int even_index = 0;  
  int odd_index = 0;  
  
  for (int i = 0; i < length; i++) {  
    if (i % 2 == 0 && even_index < even) {  
      even_arr[even_index] = s[i];  
      even_index += 1;  
    }  
  
    else {  
      if (i % 2 != 0 && odd_index < odd) {  
        odd_arr[odd_index] = s[i];  
        odd_index += 1;  
      }  
    }  
  }  
  
  // pointer to pointer of ints  
  // so essentially an array where the first element points to an array of ints  
  // we want to allocate space for two such arrays  
  int **arr;  
  arr = malloc(sizeof(int*) * 2);  
  arr[0] = even_arr;  
  arr[1] = odd_arr;  
  return arr;  
}  
  
/* Return a pointer to an array of ints with size elements. 
   - strs is an array of strings where each element is the string 
     representation of an integer. 
   - size is the size of the array 
 */  
  
int *build_array(char **strs, int size) {  
  int *full_array;  
  full_array = malloc(sizeof(int) * size);  
  for (int i = 0; i < size; i++) {  
    full_array[i] = strtol(strs[i], NULL, 10);  
  }  
  return full_array;  
}  
  
  
int main(int argc, char **argv) {  
    /* Replace the comments in the next two lines with the appropriate 
       arguments.  Do not add any additional lines of code to the main 
       function or make other changes. 
     */  

    // ./split_array 1 2 3
    int *full_array = build_array(&(argv[1]), argc - 1);  
    int **result = split_array(full_array, argc - 1);  
  
    printf("Original array:\n");  
    for (int i = 0; i < argc - 1; i++) {  
        printf("%d ", full_array[i]);  
    }  
    printf("\n");  
  
    printf("result[0]:\n");  
    for (int i = 0; i < argc / 2; i++) {  
        printf("%d ", result[0][i]);  
    }  
    printf("\n");  
  
    printf("result[1]:\n");  
    for (int i = 0; i < (argc - 1) / 2; i++) {  
        printf("%d ", result[1][i]);  
    }  
    printf("\n");  
    free(full_array);  
    free(result[0]);  
    free(result[1]);  
    free(result);  
    return 0;  
}  