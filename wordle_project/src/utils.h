#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#define WORD_LENGTH 5

// Structure to hold the dictionary
typedef struct {
    char **words;
    int count;
    int capacity;
} Dictionary;

// Function prototypes
Dictionary* load_dictionary(const char *filename);
void free_dictionary(Dictionary *dict);
bool is_valid_guess(const char *guess, const Dictionary *dict);
char* get_random_word(const Dictionary *dict);
void to_lowercase(char *str);

#endif
