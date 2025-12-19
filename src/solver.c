#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

/**
 * @brief Checks if a dictionary word is consistent with the feedback received.
 * 
 * @param word The candidate word from the dictionary.
 * @param guess The guess that generated the feedback.
 * @param feedback The feedback received for the guess.
 * @return true if the word could be the target, false otherwise.
 */
bool is_consistent(const char *word, const char *guess, const FeedbackType *feedback) {
    FeedbackType temp_feedback[WORD_LENGTH];
    get_feedback(word, guess, temp_feedback);
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (temp_feedback[i] != feedback[i]) return false;
    }
    return true;
}

/**
 * @brief Calculates a heuristic score for a word based on letter frequency.
 * 
 * @param word The word to score.
 * @param letter_counts Array of letter frequencies in the current candidate set.
 * @return int The calculated score.
 */
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

/**
 * @brief Selects the best next guess from the dictionary.
 * 
 * Strategy:
 * 1. Count letter frequencies across all remaining valid candidates.
 * 2. Score each candidate based on how many high-frequency letters it contains.
 * 3. Choose the word with the highest score to maximize information gain.
 * 
 * @param dict The full dictionary.
 * @param candidates Array of indices of valid candidates in the dictionary.
 * @param candidate_count Number of valid candidates.
 * @return char* The selected guess (must be freed by caller).
 */
char* choose_best_guess(const Dictionary *dict, int *candidates, int candidate_count) {
    // Optimization: If only one candidate remains, that must be the answer.
    if (candidate_count == 1) {
        return strdup(dict->words[candidates[0]]);
    }

    // Step 1: Calculate letter frequencies
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

    // Step 2: Find the word with the highest score
    // Current Strategy: Greedy selection from candidates.
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
    printf("\n=== WORDLE SOLVER ===\n");
    printf("Target: %s\n\n", target_word);

    int *candidates = malloc(dict->count * sizeof(int));
    int candidate_count = dict->count;
    for (int i = 0; i < dict->count; i++) candidates[i] = i;

    int attempts = 0;
    char *guess = NULL;
    FeedbackType feedback[WORD_LENGTH];

    while (attempts < 10) { // Safety limit
        attempts++;
        
        // Strategy: Start with a strong opener like "trace" or "crate"
        if (attempts == 1) {
            guess = strdup("trace"); 
            if (!is_valid_guess(guess, dict)) {
                free(guess);
                guess = choose_best_guess(dict, candidates, candidate_count);
            }
        } else {
            guess = choose_best_guess(dict, candidates, candidate_count);
        }

        printf("Guess %d: %s\n", attempts, guess);

        if (strcmp(guess, target_word) == 0) {
            printf("\n\033[1;32mSolved in %d attempts!\033[0m\n", attempts);
            free(guess);
            break;
        }

        get_feedback(target_word, guess, feedback);
        print_feedback(guess, feedback); // Uses shared game.c function for colored output

        // Filter candidates
        int new_count = 0;
        for (int i = 0; i < candidate_count; i++) {
            if (is_consistent(dict->words[candidates[i]], guess, feedback)) {
                candidates[new_count++] = candidates[i];
            }
        }
        candidate_count = new_count;
        printf("Possible words left: %d\n", candidate_count);

        free(guess);
        
        if (candidate_count == 0) {
            printf("Error: No candidates left. Dictionary might be missing the target word.\n");
            break;
        }
        printf("---------------------\n");
    }

    free(candidates);
}
