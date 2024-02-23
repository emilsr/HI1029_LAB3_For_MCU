#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <float.h>

#define PIECE_1 0b1100001
#define PIECE_2 0b1100010
#define PIECE_3 0b1000011
#define PIECE_4 0b100011

#define PIECE_2_ALT 0b110001

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

/**
 * @brief A function that gets the next valid position (0-4 in each row), doesnt care about if the bit is occupied or not.
 * 
 * @param pos The current position.
 * @return uint32_t The next position.
 */
uint32_t next(uint32_t pos) {
    return (((pos + 2) % 5) == 0) ? (uint32_t)(pos + 2) : (uint32_t)(pos + 1);
}

/**
 * @brief A function that gets the next empty position.
 * 
 * @param board the current board.
 * @param pos the current position.
 * @return uint32_t the next empty position.
 */
uint32_t nextEmpty(uint32_t board, uint32_t pos) {
    /* Used this at first
    while((board >> pos & 1)) pos++;
    return pos;
    */
    return pos + (uint32_t)__builtin_ctz(~(board >> pos));
}
/**
 * @brief A function that prints the board.
 * 
 * @param board The board (an int with bits representing slots in the board)
 */
void printBoard(uint32_t board) {
    for(int i = 0; i < 25; i++) {
        printf("%d, ", board>>(i) & 1);
        if((i+1)%5==0) {
            printf("\n");
        }
    }
    printf("\n");
}

/**
 * @brief Goes through each allowed spot (0-4 in each row) and checks if a piece can be placed, else dont place and just increment position. (more loops).
 * 
 * @param board the board, uses 25 of lsb in the 32bit integer.
 * @param pos the current position on the board, should initially be set to 0.
 * @param piecesPlaced the nr of pieces placed, should initially be set to 0.
 * @return uint8_t the nr of solutions found.
 */
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

/**
 * @brief Jumps to the next free spot (less loops).
 * 
 * @param board the board, uses 25 of lsb in the 32bit integer.
 * @param pos the current position on the board, should initially be set to 0.
 * @param piecesPlaced the nr of pieces placed, should initially be set to 0.
 * @return uint8_t the nr of solutions found.
 */
uint8_t solve2(uint32_t board, uint32_t pos, uint8_t piecesPlaced) {
    pos = nextEmpty(board, pos);

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
    if((pos+1) % 5 != 0) {              // får inte vara sista i en rad
        if(canPlace(board, pos, PIECE_1)) {
            nrOfSolutions += solve2(put(board, pos, PIECE_1), pos+1, piecesPlaced+1);
        }
        if(canPlace(board, pos, PIECE_3)) {
            nrOfSolutions += solve2(put(board, pos, PIECE_3), pos+2, piecesPlaced+1);
        }
        if(canPlace(board, pos, PIECE_4)) {
            nrOfSolutions += solve2(put(board, pos, PIECE_4), pos+2, piecesPlaced+1);
        }
    }
    if((pos) % 5 != 0) {    // får inte vara första i en rad
        if(canPlace(board, pos, PIECE_2_ALT)) {
            nrOfSolutions += solve2(put(board, pos, PIECE_2_ALT), pos+1, piecesPlaced+1);
        }
    }
    return nrOfSolutions;
}


void doSolveBench2(uint32_t i, double times[25][40], int trial) {
    struct timespec start, end;
    uint32_t board = put(0, i, 1);
    clock_gettime(CLOCK_MONOTONIC, &start);

    solve2(board, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    times[i][trial] = elapsed_ns;
}

void doSolveBench(uint32_t i, double times[25][40], int trial) {
    struct timespec start, end;
    uint32_t board = put(0, i, 1);
    clock_gettime(CLOCK_MONOTONIC, &start);

    solve(board, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    times[i][trial] = elapsed_ns;
}

void getResults(double times[25][40], int pos, double *pSum, double *pMax, double *pMin, int nrOfRuns) {
    for (int trial = 0; trial < nrOfRuns; trial++) {
        double time = times[pos][trial];
        (*pSum) += time;
        if (time > (*pMax)) {
            (*pMax) = time;
        }
        if (time < (*pMin)) {
            (*pMin) = time;
        }
    }
}

void bench(uint8_t use_min_loop) {
    double times[25][40] = {0};
    int num_trials = 40;

    for (int trial = 0; trial < num_trials; trial++) {
        for (int i = 0; i < 25; i++) {
            if(use_min_loop) {
                doSolveBench2(i, times, trial);
            } doSolveBench(i, times, trial);
        }
    }
    if(use_min_loop) {
        printf("\nBenchmark Results with less looping:\n");
    } else {
        printf("\nBenchmark Results:\n");
    }
    for (int i = 0; i < 25; i++) {
        double sum = 0;
        double max_time = 0;
        double min_time = DBL_MAX; // Set initial min time to the highest possible double value

        getResults(times, i, &sum, &max_time, &min_time, num_trials);

        double avg_time = sum / num_trials;
        printf("Position %d:\tAvg: %f ns,\tMax: %f ns,\tMin: %f ns\n", i, avg_time, max_time, min_time);
    }
}

void doSolve2(uint32_t i) {
    uint32_t board = put(0, i, 1);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    uint8_t solutions = solve2(board, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    printf("Found: %d solutions\t", solutions);
    printf("in: %f nanoseconds\t REDUCED LOOPS\n", elapsed_ns);
}

void doSolve(uint32_t i) {
    uint32_t board = put(0, i, 1);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    uint8_t solutions = solve(board, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    printf("Found: %d solutions\t", solutions);
    printf("in: %f nanoseconds\n", elapsed_ns);
}

int main(int argc, char **argv) {
    printf("DOING NORMAL SOLVING:\n");
    for(int i = 0; i < 25; i++) {
        doSolve(i);
        doSolve2(i);
    }
    printf("Starting BENCHMARKs:\n");
    bench(0);
    bench(1);
    
    return 0;
}


/* UNUSED CODE
uint8_t canBeCovered(uint32_t board, uint32_t pos) {
    if(pos > 6) {
        if(canPlace(board, pos-6, PIECE_1)) {
            return 1;
        }
        if(canPlace(board, pos-6, PIECE_2)) {
            return 1;
        }
        if(canPlace(board, pos-6, PIECE_3)) {
            return 1;
        }
    }
    if(pos > 5) {
        if(canPlace(board, pos-5, PIECE_1)) {
            return 1;
        }
        if(canPlace(board, pos-5, PIECE_2)) {
            return 1;
        }
        if(canPlace(board, pos-5, PIECE_4)) {
            return 1;
        }
    }
    if(pos > 1) {
        if(canPlace(board, pos-1, PIECE_2)) {
            return 1;
        }
        if(canPlace(board, pos-1, PIECE_3)) {
            return 1;
        }
        if(canPlace(board, pos-1, PIECE_4)) {
            return 1;
        }
    }
    if(canPlace(board, pos, PIECE_1)) {
            return 1;
    }
    if(canPlace(board, pos, PIECE_3)) {
        return 1;
    }
    if(canPlace(board, pos, PIECE_4)) {
        return 1;
    }
    return 0;    
}
*/