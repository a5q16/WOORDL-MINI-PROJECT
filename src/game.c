#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "game.h"

// ANSI Color Codes
#define COLOR_RESET "\033[0m"
#define BG_GREEN    "\033[42;30m" // Green Background, Black Text
#define BG_YELLOW   "\033[43;30m" // Yellow Background, Black Text
#define BG_GRAY     "\033[100;37m" // Dark Gray Background, White Text
#define BG_DEFAULT  "\033[40;37m" // Black Background, White Text
#define TERM_CLEAR  "\033[2J\033[H"

/**
 * @brief Generates feedback for a guess against a target word.
 * 
 * @param target The secret target word.
 * @param guess The player's guess.
 * @param feedback Output array to store the feedback (Green/Yellow/Gray).
 */
void get_feedback(const char *target, const char *guess, FeedbackType *feedback) {
    int target_counts[26] = {0};
    int guess_matched[WORD_LENGTH] = {0}; 
    int target_matched[WORD_LENGTH] = {0}; 

    // Initialize feedback
    for (int i = 0; i < WORD_LENGTH; i++) feedback[i] = FEEDBACK_GRAY;

    // 1. Check for Green
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = FEEDBACK_GREEN;
            guess_matched[i] = 1;
            target_matched[i] = 1;
        }
    }

    // Count remaining target letters
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (!target_matched[i]) {
            target_counts[target[i] - 'a']++;
        }
    }

    // 2. Check for Yellow
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess_matched[i]) continue; 

        int char_idx = guess[i] - 'a';
        if (target_counts[char_idx] > 0) {
            feedback[i] = FEEDBACK_YELLOW;
            target_counts[char_idx]--;
        }
    }
}

void print_feedback(const char *guess, const FeedbackType *feedback) {
    // Legacy function support if needed, but we mostly use render_board now.
    // We can keep it simple or redirect to a single-line render.
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (feedback[i] == FEEDBACK_GREEN) printf("%s %c %s", BG_GREEN, toupper(guess[i]), COLOR_RESET);
        else if (feedback[i] == FEEDBACK_YELLOW) printf("%s %c %s", BG_YELLOW, toupper(guess[i]), COLOR_RESET);
        else printf("%s %c %s", BG_GRAY, toupper(guess[i]), COLOR_RESET);
        printf(" ");
    }
    printf("\n");
}

void render_board(char guesses[6][WORD_LENGTH + 1], FeedbackType feedbacks[6][WORD_LENGTH], int current_attempt, const char *message) {
    printf("%s", TERM_CLEAR);
    printf("W O R D L E\n");
    printf("-----------\n\n");

    for (int i = 0; i < 6; i++) {
        if (i < current_attempt) {
            // Render guessed row
            for (int j = 0; j < WORD_LENGTH; j++) {
                char letter = toupper(guesses[i][j]);
                if (feedbacks[i][j] == FEEDBACK_GREEN) {
                    printf("%s %c %s ", BG_GREEN, letter, COLOR_RESET);
                } else if (feedbacks[i][j] == FEEDBACK_YELLOW) {
                    printf("%s %c %s ", BG_YELLOW, letter, COLOR_RESET);
                } else {
                    printf("%s %c %s ", BG_GRAY, letter, COLOR_RESET);
                }
            }
        } else {
            // Render empty row
            for (int j = 0; j < WORD_LENGTH; j++) {
                printf("[ ] "); 
            }
        }
        printf("\n\n");
    }

    if (message && strlen(message) > 0) {
        printf("> %s\n", message);
    }
}

void play_game(const char *target_word, const Dictionary *dict) {
    char guesses[6][WORD_LENGTH + 1];
    FeedbackType feedbacks[6][WORD_LENGTH];
    int attempt = 0;
    int won = 0;
    char message[128] = "Enter a 5-letter word:";

    // Initialize memory
    memset(guesses, 0, sizeof(guesses));
    memset(feedbacks, 0, sizeof(feedbacks));

    while (attempt < 6) {
        render_board(guesses, feedbacks, attempt, message);

        char input[64];
        if (!fgets(input, sizeof(input), stdin)) break;

        // Clean input
        input[strcspn(input, "\n")] = 0;
        input[strcspn(input, "\r")] = 0;
        
        // Command to quit
        if (strcmp(input, ":q") == 0) {
            printf("Quitting game.\n");
            return;
        }

        if (strlen(input) != WORD_LENGTH) {
            snprintf(message, sizeof(message), "Invalid length! Must be 5 letters.");
            continue;
        }

        to_lowercase(input);
        
        // Validate word
        bool in_dict = is_valid_guess(input, dict);
        if (!in_dict) {
            snprintf(message, sizeof(message), "'%s' is not in the word list.", input);
            continue;
        }

        // Process Valid Guess
        strcpy(guesses[attempt], input);
        get_feedback(target_word, guesses[attempt], feedbacks[attempt]);
        
        // Check Win
        int greens = 0;
        for(int k=0; k<WORD_LENGTH; k++) 
            if(feedbacks[attempt][k] == FEEDBACK_GREEN) greens++;
        
        if (greens == WORD_LENGTH) {
            won = 1;
            attempt++;
            break; 
        }

        attempt++;
        snprintf(message, sizeof(message), "Enter guess %d/6:", attempt + 1);
    }

    render_board(guesses, feedbacks, attempt, "");
    
    if (won) {
        printf("\n\033[1;32mImpressive! You guessed '%s' in %d attempts.\033[0m\n", target_word, attempt);
    } else {
        printf("\n\033[1;31mGame Over! The word was '%s'.\033[0m\n", target_word);
    }
}
