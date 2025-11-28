# Wordle Project Report

## Part 1: Strategy Description

### Word Selection Strategy
The solver uses a **Letter Frequency Heuristic** to select the best guess.
1.  **Candidate Filtering**: After each guess, the solver eliminates words from the candidate list that are inconsistent with the received feedback. For example, if a letter is marked "Gray", all words containing that letter are removed (unless the letter appears elsewhere as Green/Yellow).
2.  **Guess Selection**: To choose the next guess, the solver calculates the frequency of each letter (A-Z) across all remaining candidate words. It then scores each candidate word by summing the frequencies of its unique letters. The word with the highest score is chosen.
    *   *Rationale*: This approach maximizes the probability of finding correct letters (Green/Yellow) in the unknown target word, thereby reducing the search space quickly.

### Effectiveness
This strategy is effective because it prioritizes "most likely" letters. By guessing words composed of high-frequency letters, we are more likely to get positive feedback (Green/Yellow) or, conversely, eliminate a large chunk of words if the common letters are absent (Gray). While not as optimal as Information Theoretic (Entropy) or Minimax approaches, it is computationally efficient and sufficient to solve Wordle within 6 guesses in the vast majority of cases.

## Part 2: Data Structure Justification

### Data Structures Used
1.  **Dictionary (`char **words`)**:
    *   **Type**: Dynamic Array of Strings.
    *   **Justification**: We need to store the full list of valid words. A dynamic array allows us to load any number of words from the file. Access by index is O(1).
2.  **Candidate List (`int *candidates`)**:
    *   **Type**: Dynamic Array of Integers (Indices).
    *   **Justification**: Instead of copying strings to create a new list of candidates every turn, we maintain a list of *indices* pointing to the original dictionary. This reduces memory usage and copying overhead.
3.  **Feedback Array (`enum FeedbackType`)**:
    *   **Type**: Array of Enums.
    *   **Justification**: Represents the state of each letter (Green, Yellow, Gray) clearly and safely.

### Alternatives Considered
*   **Linked List for Candidates**: Would allow O(1) removal, but traversal is O(N) and cache locality is poor. Array of indices is better for iteration.
*   **Bitmask for Letter Presence**: Could be used for faster filtering, but 5-letter words are short enough that string operations are fast.

## Part 3: Complexity Analysis

### Time Complexity
*   **Initialization**: O(W * L), where W is the number of words and L is word length (5).
*   **Filtering**: O(C * L), where C is the number of remaining candidates. In the worst case, C starts at W.
*   **Guess Selection**: O(C * L) to calculate frequencies and scores.
*   **Total per Turn**: O(C * L). Since C decreases rapidly, the solver is very fast.

### Space Complexity
*   **Dictionary**: O(W * L) to store all words.
*   **Candidates**: O(W) to store indices.
*   **Total**: O(W * L). For ~13,000 words, this is negligible (~70KB).

## Part 4: Code Documentation

### `src/utils.c`
*   `load_dictionary(filename)`: Reads the file line-by-line, allocates memory dynamically, and returns a `Dictionary` struct.
*   `is_valid_guess(guess, dict)`: Performs a linear search to check if a word exists.

### `src/game.c`
*   `get_feedback(target, guess, feedback)`: Core logic.
    *   First pass: Mark exact matches (Green).
    *   Second pass: Mark partial matches (Yellow) using a frequency count of remaining target letters to handle duplicates correctly.
    *   Remaining are Gray.

### `src/solver.c`
*   `solve_game(target, dict)`: Main solver loop.
    *   Initializes candidates to all words.
    *   Loops until solved or max attempts.
    *   Calls `choose_best_guess` and `filter_candidates`.
*   `choose_best_guess(dict, candidates, count)`:
    ```c
    // Example logic snippet
    // Calculate letter frequencies in remaining candidates
    for (int i = 0; i < candidate_count; i++) {
        // ... count letters ...
    }
    // Score each word
    for (int i = 0; i < candidate_count; i++) {
        int score = calculate_score(dict->words[candidates[i]], letter_counts);
        if (score > best_score) { ... }
    }
    ```
