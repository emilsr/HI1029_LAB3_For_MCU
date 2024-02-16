#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DIRECTION_N 1
#define DIRECTION_W 2
#define DIRECTION_E 3
#define DIRECTION_S 4
#define DIRECTION_NONE 0

typedef struct {
    uint8_t *grid;
    uint8_t rows;
    uint8_t cols;
    uint8_t bitsPerChar;
    uint8_t size;
} Grid;

Grid *createGrid(uint8_t rows, uint8_t cols) {
    Grid *newGrid = (Grid *)malloc(sizeof(Grid));
    if (newGrid == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    newGrid->rows = rows;
    newGrid->cols = cols;
    newGrid->bitsPerChar = sizeof(char) * 8;
    newGrid->size = (int)((rows * cols) / newGrid->bitsPerChar) + 1;
    newGrid->grid = (uint8_t *)calloc(newGrid->size, sizeof(uint8_t));
    if (newGrid->grid == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    return newGrid;
}

void set(Grid *grid, uint8_t row, uint8_t col) {
    int index = row * grid->cols + col;
    int charIndex = index / grid->bitsPerChar;
    int bitOffset = index % grid->bitsPerChar;
    grid->grid[charIndex] |= (1 << bitOffset);
}

void clear(Grid *grid, uint8_t row, uint8_t col) {
    uint8_t index = row * grid->cols + col;
    uint8_t charIndex = index / grid->bitsPerChar;
    uint8_t bitOffset = index % grid->bitsPerChar;
    grid->grid[charIndex] &= ~(1 << bitOffset);
}

uint8_t check(Grid *grid, uint8_t row, uint8_t col) {
    uint8_t index = row * grid->cols + col;
    uint8_t charIndex = index / grid->bitsPerChar;
    uint8_t bitOffset = index % grid->bitsPerChar;
    return (grid->grid[charIndex] & (1 << bitOffset)) != 0;
}

uint8_t isSolved(Grid *grid) {
    uint8_t lastRowBits = (grid->rows * grid->cols) % (grid->bitsPerChar);
    for (int i = 0; i < (grid->size - 1); i++) {
        if (grid->grid[i] != 0xFF) {
            return 0; // false
        }
    }
    uint8_t lastRowMask = (1 << lastRowBits) - 1;
    return (grid->grid[grid->size - 1] == lastRowMask) ? 1 : 0;
}

uint8_t canPlace(Grid *grid, uint8_t row, uint8_t col, uint8_t direction) {
    if (row + 1 >= grid->rows || col + 1 >= grid->cols) {
        return 0; // false
    }
    
    switch (direction) {
        case 1: // N
            return (!check(grid, row, col) &&
                    !check(grid, row + 1, col) &&
                    !check(grid, row + 1, col + 1)) ? 1 : 0;
        case 4: // S
            return (!check(grid, row + 1, col) &&
                    !check(grid, row, col) &&
                    !check(grid, row, col + 1)) ? 1 : 0;
        case 2: // W
            return (!check(grid, row, col + 1) &&
                    !check(grid, row + 1, col) &&
                    !check(grid, row + 1, col + 1)) ? 1 : 0;
        case 3: // E
            return (!check(grid, row, col) &&
                    !check(grid, row, col + 1) &&
                    !check(grid, row + 1, col + 1)) ? 1 : 0;
        case 0: // NONE
            return 0; // false
        default:
            return 0; // false
    }
}

void place(Grid *grid, uint8_t row, uint8_t col, uint8_t direction) {
    switch (direction) {
        case 1: // N
            set(grid, row, col);
            set(grid, row + 1, col);
            set(grid, row + 1, col + 1);
            break;
        case 4: // S
            set(grid, row + 1, col);
            set(grid, row, col);
            set(grid, row, col + 1);
            break;
        case 2: // W
            set(grid, row, col + 1);
            set(grid, row + 1, col);
            set(grid, row + 1, col + 1);
            break;
        case 3: // E
            set(grid, row, col);
            set(grid, row, col + 1);
            set(grid, row + 1, col + 1);
            break;
        case 0: // NONE
            // Do nothing
            break;
    }
}

void unPlace(Grid *grid, uint8_t row, uint8_t col, uint8_t direction) {
    switch (direction) {
        case 1: // N
            clear(grid, row, col);
            clear(grid, row + 1, col);
            clear(grid, row + 1, col + 1);
            break;
        case 4: // S
            clear(grid, row + 1, col);
            clear(grid, row, col);
            clear(grid, row, col + 1);
            break;
        case 2: // W
            clear(grid, row, col + 1);
            clear(grid, row + 1, col);
            clear(grid, row + 1, col + 1);
            break;
        case 3: // E
            clear(grid, row, col);
            clear(grid, row, col + 1);
            clear(grid, row + 1, col + 1);
            break;
        case 0: // NONE
            // Do nothing
            break;
    }
}

uint8_t isUnsolvable(Grid *grid, uint8_t row, uint8_t col) {
    return col>0?!check(grid, row, col-1) : row > 0 && check(grid, row - 1, grid->cols -1);
}

uint8_t solve(Grid *grid, uint8_t row, uint8_t col, uint8_t piecesPlaced) {
    if(piecesPlaced >= 8) {
        if(isSolved(grid)) {
            return 1;
        }
        return 0;
    }

    if(isUnsolvable(grid, row, col)) {
        return 0;
    }

    if(col >= grid->cols) {
        row++;
        col = 0;
    }
    if(row >= grid->rows) {
        return 0;
    }

    uint8_t solutions = 0;

    if(canPlace(grid, row, col, DIRECTION_N)) {
        place(grid, row, col, DIRECTION_N);
        solutions += solve(grid, row, col + 1, piecesPlaced + 1);
        unPlace(grid, row, col, DIRECTION_N);
    }

    if(canPlace(grid, row, col, DIRECTION_W)) {
        place(grid, row, col, DIRECTION_W);
        solutions += solve(grid, row, col + 1, piecesPlaced + 1);
        unPlace(grid, row, col, DIRECTION_W);
    }

    if(canPlace(grid, row, col, DIRECTION_E)) {
        place(grid, row, col, DIRECTION_E);
        solutions += solve(grid, row, col + 1, piecesPlaced + 1);
        unPlace(grid, row, col, DIRECTION_E);
    }

    if(canPlace(grid, row, col, DIRECTION_S)) {
        place(grid, row, col, DIRECTION_S);
        solutions += solve(grid, row, col + 1, piecesPlaced + 1);
        unPlace(grid, row, col, DIRECTION_S);
    }

    return solutions + solve(grid, row, col + 1, piecesPlaced);
}

void getSolutions(uint8_t row, uint8_t col){
    printf("%d,%d\t:\t", row+1, col+1);
    Grid *pGrid = createGrid(5,5);
    set(pGrid, row, col);

    clock_t start = clock();
    uint8_t solutions = solve(pGrid, 0, 0, 0);
    clock_t end = clock();
    
    double time_taken = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;

    printf("Found: %d solutions\t", solutions);
    printf("in: %f milliseconds\n", time_taken);
}

int main() {
    for(uint8_t i = 0; i < 5; i++) {
        for(uint8_t j = 0; j < 5; j++) {
            getSolutions(i,j);
        }
    }

    return 0;
}