#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "game.h"
#include "solver.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s [mode] [args]\n", prog_name);
    printf("Modes:\n");
    printf("  (no args)           Play Wordle with a random word\n");
    printf("  <word>              Play Wordle with a specific target word\n");
    printf("  --solve             Run solver with a random word\n");
    printf("  --solve <word>      Run solver with a specific target word\n");
    printf("  --benchmark <N>     Run solver on N random words\n");
}

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

void clear_screen() {
    printf("\033[2J\033[H");
}

void print_menu() {
    clear_screen();
    printf("\033[1;36m=============================\n");
    printf("   W O R D L E   M A S T E R \n");
    printf("=============================\033[0m\n\n");
    printf("1. \033[1;32mPlay Wordle\033[0m\n");
    printf("2. \033[1;33mWordle Solver\033[0m\n");
    printf("3. Exit\n\n");
    printf("Select mode: ");
}

void wait_for_enter() {
    printf("\n\033[90mPress Enter to return to menu...\033[0m");
    while (getchar() != '\n'); // Clear buffer if needed
    getchar(); // Wait for actual enter
}

int main(int argc, char *argv[]) {
    // Enable ANSI escape codes on Windows
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
    #endif

    srand(time(NULL));

    Dictionary *dict = load_dictionary("words.txt");
    if (!dict) {
        fprintf(stderr, "Error: Could not load dictionary 'words.txt'. Make sure it exists.\n");
        return 1;
    }

    // Argument Mode (Non-Interactive)
    if (argc > 1) {
        if (strcmp(argv[1], "--solve") == 0) {
            char *target;
            if (argc > 2) {
                target = strdup(argv[2]);
            } else {
                target = get_random_word(dict);
            }
            solve_game(target, dict);
            free(target);
        } else if (strcmp(argv[1], "--benchmark") == 0) {
            int n = 100;
            if (argc > 2) n = atoi(argv[2]);
            printf("Running benchmark on %d words...\n", n);
            for(int i=0; i<n; i++) {
                char *t = get_random_word(dict);
                solve_game(t, dict);
                free(t);
                printf("--------------------------------------------------\n");
            }
        } else {
            // Assume argument is a target word
            char *target = strdup(argv[1]);
            play_game(target, dict);
            free(target);
        }
        free_dictionary(dict);
        return 0;
    }

    // Interactive Mode
    int choice;
    char input_buffer[64];
    
    while (1) {
        print_menu();
        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) break;
        
        if (sscanf(input_buffer, "%d", &choice) != 1) {
            continue;
        }

        if (choice == 1) {
            char *target = get_random_word(dict);
            play_game(target, dict);
            free(target);
            wait_for_enter();
        } else if (choice == 2) {
            char *target = get_random_word(dict);
            solve_game(target, dict);
            free(target);
            wait_for_enter();
        } else if (choice == 3) {
            printf("Thanks for playing!\n");
            break;
        }
    }

    free_dictionary(dict);
    return 0;
}
