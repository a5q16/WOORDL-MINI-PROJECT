# Wordle Project

This is a C implementation of the Wordle game and an automated solver.

## Project Structure
- `src/`: Source code files (`main.c`, `game.c`, `solver.c`, `utils.c` and headers).
- `words.txt`: Dictionary of valid 5-letter words.
- `Makefile`: Build script for Make (optional).
- `report.md`: Analysis and documentation.

## How to Build
You need a C compiler (like GCC) installed.

### Using GCC directly:
```bash
gcc -Wall -std=c99 src/main.c src/utils.c src/game.c src/solver.c -o wordle
```

### Using Make (if available):
```bash
make
```

## How to Run

### Play the Game
Run the program to play a random game:
```bash
./wordle
```
Or play with a specific word:
```bash
./wordle apple
```

### Run the Solver
Run the automated solver on a random word:
```bash
./wordle --solve
```
Run the solver on a specific word:
```bash
./wordle --solve crane
```

## Example Output
```
Welcome to Wordle! Guess the 5-letter word.
Attempt 1/6: trace
[Gray]t [Gray]r [Yellow]a [Gray]c [Yellow]e
Attempt 2/6: ...
```
