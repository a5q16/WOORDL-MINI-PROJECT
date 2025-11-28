#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

// Helper to check if a word matches the feedback given a guess
bool is_consistent(const char *word, const char *guess, const FeedbackType *feedback) {
    FeedbackType temp_feedback[WORD_LENGTH];
    get_feedback(word, guess, temp_feedback);
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (temp_feedback[i] != feedback[i]) return false;
    }
    return true;
}

// Calculate score for a word based on letter frequency in candidates
int calculate_score(const char *word, int *letter_counts) {
    int score = 0;
    bool seen[26] = {false};
    for (int i = 0; i < WORD_LENGTH; i++) {
        int idx = word[i] - 'a';
        if (!seen[idx]) {
            score += letter_counts[idx];
            seen[idx] = true;
        }
    }
    return score;
}

char* choose_best_guess(const Dictionary *dict, int *candidates, int candidate_count) {
    // If only 1 candidate, return it
    if (candidate_count == 1) {
        return strdup(dict->words[candidates[0]]);
    }

    // Calculate letter frequencies in remaining candidates
    int letter_counts[26] = {0};
    for (int i = 0; i < candidate_count; i++) {
        const char *w = dict->words[candidates[i]];
        bool seen_in_word[26] = {false};
        for (int j = 0; j < WORD_LENGTH; j++) {
            int idx = w[j] - 'a';
            if (!seen_in_word[idx]) {
                letter_counts[idx]++;
                seen_in_word[idx] = true;
            }
        }
    }

    // Find word with highest score
    // We can search the entire dictionary for the best guess (Minimax style), 
    // or just the candidates. Searching candidates is faster and usually sufficient.
    // For better performance, searching the whole dictionary is often better to eliminate more,
    // but let's stick to candidates for simplicity and speed first.
    // Actually, for the very first guess, we should pick a known good word or calculate it.
    // Let's search ALL valid words for the best guess, but prioritize candidates if scores are close?
    // No, let's just search candidates for now.
    
    int best_score = -1;
    int best_index = -1;

    for (int i = 0; i < candidate_count; i++) {
        int idx = candidates[i];
        int score = calculate_score(dict->words[idx], letter_counts);
        if (score > best_score) {
            best_score = score;
            best_index = idx;
        }
    }

    return strdup(dict->words[best_index]);
}

void solve_game(const char *target_word, const Dictionary *dict) {
    printf("Solver started for target: %s\n", target_word);

    int *candidates = malloc(dict->count * sizeof(int));
    int candidate_count = dict->count;
    for (int i = 0; i < dict->count; i++) candidates[i] = i;

    int attempts = 0;
    char *guess = NULL;
    FeedbackType feedback[WORD_LENGTH];

    while (attempts < 10) { // Safety limit
        attempts++;
        
        // First guess optimization
        if (attempts == 1) {
            guess = strdup("trace"); // A standard good starting word
            if (!is_valid_guess(guess, dict)) {
                free(guess);
                guess = choose_best_guess(dict, candidates, candidate_count);
            }
        } else {
            guess = choose_best_guess(dict, candidates, candidate_count);
        }

        printf("Guess %d: %s\n", attempts, guess);

        if (strcmp(guess, target_word) == 0) {
            printf("Solver found the word in %d attempts!\n", attempts);
            free(guess);
            break;
        }

        get_feedback(target_word, guess, feedback);
        print_feedback(guess, feedback);

        // Filter candidates
        int new_count = 0;
        for (int i = 0; i < candidate_count; i++) {
            if (is_consistent(dict->words[candidates[i]], guess, feedback)) {
                candidates[new_count++] = candidates[i];
            }
        }
        candidate_count = new_count;
        printf("Remaining candidates: %d\n", candidate_count);

        free(guess);
        
        if (candidate_count == 0) {
            printf("Error: No candidates left. Dictionary might be missing the target word.\n");
            break;
        }
    }

    free(candidates);
}
