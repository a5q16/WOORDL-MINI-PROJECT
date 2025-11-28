#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

// ANSI Color Codes
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GRAY "\033[90m"
#define COLOR_RESET "\033[0m"

void get_feedback(const char *target, const char *guess, FeedbackType *feedback) {
    int target_counts[26] = {0};
    int guess_matched[WORD_LENGTH] = {0}; // 1 if this index in guess is already matched (Green)
    int target_matched[WORD_LENGTH] = {0}; // 1 if this index in target is already matched (Green)

    // Initialize feedback to Gray
    for (int i = 0; i < WORD_LENGTH; i++) {
        feedback[i] = FEEDBACK_GRAY;
    }

    // 1. Check for Green (Correct position)
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = FEEDBACK_GREEN;
            guess_matched[i] = 1;
            target_matched[i] = 1;
        }
    }

    // Count remaining available letters in target
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (!target_matched[i]) {
            target_counts[target[i] - 'a']++;
        }
    }

    // 2. Check for Yellow (Wrong position)
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess_matched[i]) continue; // Already Green

        int char_idx = guess[i] - 'a';
        if (target_counts[char_idx] > 0) {
            feedback[i] = FEEDBACK_YELLOW;
            target_counts[char_idx]--;
        }
    }
}

void print_feedback(const char *guess, const FeedbackType *feedback) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == FEEDBACK_GREEN) {
            printf("%s%c%s", COLOR_GREEN, guess[i], COLOR_RESET);
        } else if (feedback[i] == FEEDBACK_YELLOW) {
            printf("%s%c%s", COLOR_YELLOW, guess[i], COLOR_RESET);
        } else {
            printf("%s%c%s", COLOR_GRAY, guess[i], COLOR_RESET);
        }
    }
    printf("\n");
}

void play_game(const char *target_word, const Dictionary *dict) {
    char guess[64];
    FeedbackType feedback[WORD_LENGTH];
    int attempts = 0;
    const int MAX_ATTEMPTS = 6;
    int won = 0;

    printf("Welcome to Wordle! Guess the 5-letter word.\n");

    while (attempts < MAX_ATTEMPTS) {
        printf("Attempt %d/%d: ", attempts + 1, MAX_ATTEMPTS);
        if (scanf("%63s", guess) != 1) break;
        
        to_lowercase(guess);

        if (!is_valid_guess(guess, dict)) {
            printf("Invalid word. Try again.\n");
            continue;
        }

        get_feedback(target_word, guess, feedback);
        print_feedback(guess, feedback);

        // Check win condition
        int green_count = 0;
        for (int i = 0; i < WORD_LENGTH; i++) {
            if (feedback[i] == FEEDBACK_GREEN) green_count++;
        }

        if (green_count == WORD_LENGTH) {
            won = 1;
            break;
        }

        attempts++;
    }

    if (won) {
        printf("Congratulations! You guessed the word '%s' in %d attempts.\n", target_word, attempts + 1);
    } else {
        printf("Game Over. The word was '%s'.\n", target_word);
    }
}
