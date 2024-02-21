#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define PIECE_1 0b1100001
#define PIECE_2 0b1100010
#define PIECE_3 0b1000011
#define PIECE_4 0b100011

uint32_t put(uint32_t board, uint32_t pos, uint32_t piece) {
    return (uint32_t)(board | (piece << pos));
}

uint32_t canPlace(uint32_t board, uint32_t pos, uint32_t piece) {
    return (uint32_t)!(board & (piece << pos));
}
/*
    0  1  2  3  4
    5  6  7  8  9
    10 11 12 13 14
    15 16 17 18 19
    20 21 22 23 24
*/
uint32_t isUnsolvable(uint32_t board, uint32_t pos) {
    if(pos < 1) return 0;
    if(pos % 5 == 0) {
        if (((board >> (pos - 2)) & 0b11) == 0b11) return 0;
    } else {
        if((board & (1 << (pos-1)))) return 0;
    }
    return (uint32_t) 1;
}

uint32_t next(uint32_t pos) {
    return (((pos + 2) % 5) == 0) ? (uint32_t)(pos + 2) : (uint32_t)(pos + 1);
}

void printBoard(uint32_t board) {
    for(int i = 0; i < 25; i++) {
        printf("%d, ", board>>(i) & 1);
        if((i+1)%5==0) {
            printf("\n");
        }
    }
    printf("\n");
}

uint8_t solve(uint32_t board, uint32_t pos, uint8_t piecesPlaced) {
    
    if(piecesPlaced >= 8) {
        return 1;
    }

    if(pos > 19) {
        return 0;
    }

    if(isUnsolvable(board, pos)) {
        return 0;
    }

    uint8_t nrOfSolutions = 0;
    if(canPlace(board, pos, PIECE_1)) {
        nrOfSolutions += solve(put(board, pos, PIECE_1), next(pos), piecesPlaced+1);
    }
    if(canPlace(board, pos, PIECE_2)) {
        nrOfSolutions += solve(put(board, pos, PIECE_2), next(pos), piecesPlaced+1);
    }
    if(canPlace(board, pos, PIECE_3)) {
        nrOfSolutions += solve(put(board, pos, PIECE_3), next(pos), piecesPlaced+1);
    }
    if(canPlace(board, pos, PIECE_4)) {
        nrOfSolutions += solve(put(board, pos, PIECE_4), next(pos), piecesPlaced+1);
    }
    return (nrOfSolutions + solve(board, next(pos), piecesPlaced));
}

void doSolve(uint32_t i) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    uint8_t solutions = solve((uint32_t)put(0, i, 1), (uint32_t)0, (uint8_t)0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    printf("Found: %d solutions\t", solutions);
    printf("in: %f nanoseconds\n", elapsed_ns);
}

int main(int argc, char **argv) {
    for(int i = 0; i < 25; i++) {
        doSolve(i);
    }
    return 0;
}