#ifndef GAME_H
#define GAME_H

#include "utils.h"

typedef enum {
    FEEDBACK_GRAY = 0,
    FEEDBACK_YELLOW = 1,
    FEEDBACK_GREEN = 2
} FeedbackType;

// Function prototypes
void get_feedback(const char *target, const char *guess, FeedbackType *feedback);
void print_feedback(const char *guess, const FeedbackType *feedback);
void play_game(const char *target_word, const Dictionary *dict);

#endif
