#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "utils.h"

Dictionary* load_dictionary(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open dictionary file");
        return NULL;
    }

    Dictionary *dict = malloc(sizeof(Dictionary));
    dict->capacity = 1000;
    dict->count = 0;
    dict->words = malloc(dict->capacity * sizeof(char*));

    char buffer[64];
    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;
        buffer[strcspn(buffer, "\r")] = 0;

        if (strlen(buffer) == WORD_LENGTH) {
            if (dict->count >= dict->capacity) {
                dict->capacity *= 2;
                dict->words = realloc(dict->words, dict->capacity * sizeof(char*));
            }
            dict->words[dict->count] = strdup(buffer);
            to_lowercase(dict->words[dict->count]);
            dict->count++;
        }
    }

    fclose(file);
    return dict;
}

void free_dictionary(Dictionary *dict) {
    if (!dict) return;
    for (int i = 0; i < dict->count; i++) {
        free(dict->words[i]);
    }
    free(dict->words);
    free(dict);
}

bool is_valid_guess(const char *guess, const Dictionary *dict) {
    if (strlen(guess) != WORD_LENGTH) return false;
    
    // Simple linear search (could be optimized with hash table or binary search if sorted)
    for (int i = 0; i < dict->count; i++) {
        if (strcmp(guess, dict->words[i]) == 0) {
            return true;
        }
    }
    return false;
}

char* get_random_word(const Dictionary *dict) {
    if (dict->count == 0) return NULL;
    int index = rand() % dict->count;
    return strdup(dict->words[index]);
}

void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}
