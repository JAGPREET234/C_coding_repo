#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define SIZE 9
#define NUM_THREADS 27
#define NUM_THREADS_NEW_METHOD 11 // 1 for rows, 1 for columns, 9 for subgrids

// Structure to pass thread data
struct thread_parameter_t {
    int thread_number;
    int puzzle_row;
    int puzzle_column;
    int puzzle_number;
    int (*sudoku)[SIZE]; // Pointer to the Sudoku puzzle array
};

// Global Sudoku puzzles
int sudoku1[SIZE][SIZE] = {
    {5, 3, 8, 9, 2, 7, 6, 1, 4},
    {9, 4, 2, 3, 5, 6, 8, 7, 1},
    {6, 1, 7, 8, 4, 2, 9, 3, 5},
    {3, 8, 6, 1, 9, 4, 2, 5, 7},
    {7, 5, 1, 6, 3, 8, 4, 9, 2},
    {4, 9, 5, 7, 8, 2, 3, 6, 1},
    {2, 7, 9, 6, 1, 3, 5, 4, 8},
    {8, 6, 3, 2, 7, 9, 1, 5, 4},
    {1, 2, 4, 5, 6, 8, 7, 9, 3}
};

int sudoku2[SIZE][SIZE] = {
    {2, 1, 9, 5, 8, 4, 3, 6, 7},
    {5, 6, 7, 3, 2, 9, 8, 4, 1},
    {8, 3, 4, 7, 6, 1, 2, 9, 5},
    {7, 5, 2, 1, 3, 6, 4, 8, 9},
    {6, 8, 3, 4, 9, 5, 7, 2, 1},
    {4, 9, 1, 8, 7, 2, 6, 3, 5},
    {3, 7, 6, 9, 5, 8, 1, 2, 4},
    {9, 4, 8, 6, 1, 7, 5, 3, 2},
    {1, 2, 5, 2, 4, 3, 9, 7, 8}
};

int sudoku3[SIZE][SIZE] = {
    {5, 8, 3, 4, 6, 9, 2, 1, 7},
    {6, 4, 9, 1, 2, 8, 5, 7, 3},
    {7, 2, 1, 5, 7, 3, 4, 9, 6},
    {1, 9, 4, 7, 8, 2, 3, 6, 5},
    {8, 6, 5, 3, 4, 1, 7, 2, 9},
    {3, 7, 2, 6, 5, 4, 9, 8, 1},
    {9, 5, 7, 2, 3, 6, 8, 4, 1},
    {4, 1, 6, 9, 8, 7, 5, 3, 2},
    {2, 3, 8, 8, 1, 5, 6, 7, 9}
};

int result[NUM_THREADS] = {0}; // Result array for each thread
int result_new_method[NUM_THREADS_NEW_METHOD] = {0}; // Result array for the new method

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to log thread activity
void log_thread_activity(int puzzle_number, int thread_number, const char *type, const char *status) {
    pthread_mutex_lock(&mutex);
    printf("Puzzle %d - Thread %d: %s validation %s\n", puzzle_number, thread_number, type, status);
    pthread_mutex_unlock(&mutex);
}

// Check if a row is valid
void* check_row(void* param) {
    struct thread_parameter_t* data = (struct thread_parameter_t*) param;
    int row = data->puzzle_row;
    bool seen[SIZE + 1] = {false};

    for (int col = 0; col < SIZE; col++) {
        int num = data->sudoku[row][col];
        if (seen[num]) {
            result[data->thread_number] = 0;
            log_thread_activity(data->puzzle_number, data->thread_number, "Row", "Invalid");
            return NULL;
        }
        seen[num] = true;
    }

    result[data->thread_number] = 1;
    log_thread_activity(data->puzzle_number, data->thread_number, "Row", "Valid");
    return NULL;
}

// Check if a column is valid
void* check_column(void* param) {
    struct thread_parameter_t* data = (struct thread_parameter_t*) param;
    int col = data->puzzle_column;
    bool seen[SIZE + 1] = {false};

    for (int row = 0; row < SIZE; row++) {
        int num = data->sudoku[row][col];
        if (seen[num]) {
            result[data->thread_number] = 0;
            log_thread_activity(data->puzzle_number, data->thread_number, "Column", "Invalid");
            return NULL;
        }
        seen[num] = true;
    }

    result[data->thread_number] = 1;
    log_thread_activity(data->puzzle_number, data->thread_number, "Column", "Valid");
    return NULL;
}

// Check if a 3x3 subgrid is valid
void* check_subgrid(void* param) {
    struct thread_parameter_t* data = (struct thread_parameter_t*) param;
    int start_row = data->puzzle_row;
    int start_col = data->puzzle_column;
    bool seen[SIZE + 1] = {false};

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int num = data->sudoku[start_row + row][start_col + col];
            if (seen[num]) {
                result[data->thread_number] = 0;
                log_thread_activity(data->puzzle_number, data->thread_number, "Subgrid", "Invalid");
                return NULL;
            }
            seen[num] = true;
        }
    }

    result[data->thread_number] = 1;
    log_thread_activity(data->puzzle_number, data->thread_number, "Subgrid", "Valid");
    return NULL;
}
// Function to check all rows in a single thread
void* check_all_rows(void* param) {
    struct thread_parameter_t* data = (struct thread_parameter_t*) param;
    int (*sudoku)[SIZE] = data->sudoku;

    for (int row = 0; row < SIZE; row++) {
        bool seen[SIZE + 1] = {false};
        for (int col = 0; col < SIZE; col++) {
            int num = sudoku[row][col];
            if (seen[num]) {
                result_new_method[data->thread_number] = 0;
                log_thread_activity(data->puzzle_number, data->thread_number, "Rows", "Invalid");
                return NULL;
            }
            seen[num] = true;
        }
    }
    result_new_method[data->thread_number] = 1;
    log_thread_activity(data->puzzle_number, data->thread_number, "Rows", "Valid");
    return NULL;
}

// Function to check all columns in a single thread
void* check_all_columns(void* param) {
    struct thread_parameter_t* data = (struct thread_parameter_t*) param;
    int (*sudoku)[SIZE] = data->sudoku;

    for (int col = 0; col < SIZE; col++) {
        bool seen[SIZE + 1] = {false};
        for (int row = 0; row < SIZE; row++) {
            int num = sudoku[row][col];
            if (seen[num]) {
                result_new_method[data->thread_number] = 0;
                log_thread_activity(data->puzzle_number, data->thread_number, "Columns", "Invalid");
                return NULL;
            }
            seen[num] = true;
        }
    }
    result_new_method[data->thread_number] = 1;
    log_thread_activity(data->puzzle_number, data->thread_number, "Columns", "Valid");
    return NULL;
}

// Function to validate a Sudoku puzzle using 11 threads
void* validate_puzzle_11_threads(void* param) {
    int puzzle_number = *((int*) param);
    pthread_t threads[NUM_THREADS_NEW_METHOD];
    struct thread_parameter_t params[NUM_THREADS_NEW_METHOD];

    int (*sudoku)[SIZE] = (puzzle_number == 1) ? sudoku1 : (puzzle_number == 2) ? sudoku2 : sudoku3;
    clock_t start_time = clock(); // Start timing the validation
    // Create threads for row, column, and subgrid validations
    params[0] = (struct thread_parameter_t){0, 0, 0, puzzle_number, sudoku};
    pthread_create(&threads[0], NULL, check_all_rows, &params[0]);

    params[1] = (struct thread_parameter_t){1, 0, 0, puzzle_number, sudoku};
    pthread_create(&threads[1], NULL, check_all_columns, &params[1]);

    // Create threads for each subgrid
    int thread_id = 2;
    for (int row = 0; row < SIZE; row += 3) {
        for (int col = 0; col < SIZE; col += 3) {
            params[thread_id] = (struct thread_parameter_t){thread_id, row, col, puzzle_number, sudoku};
            pthread_create(&threads[thread_id], NULL, check_subgrid, &params[thread_id]);
            thread_id++;
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS_NEW_METHOD; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check if the puzzle is valid
    bool valid = true;
    for (int i = 0; i < NUM_THREADS_NEW_METHOD; i++) {
        if (result_new_method[i] == 0) {
            valid = false;
            break;
        }
    }
    
    clock_t end_time = clock(); // End timing
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Display the final validation result
    if (valid) {
        printf("Puzzle %d is valid - Took %.6f secs\n", puzzle_number, time_taken);
    } else {
        printf("Puzzle %d is not valid - Took %.6f secs\n", puzzle_number, time_taken);
    }


    return NULL;
}
// Function to validate a Sudoku puzzle using multiple threads
void* validate_puzzle(void* param) {
    int puzzle_number = *((int*) param);
    printf("Starting validation for Puzzle %d using 27 threads...\n", puzzle_number);

    int (*sudoku)[SIZE] = (puzzle_number == 1) ? sudoku1 : (puzzle_number == 2) ? sudoku2 : sudoku3;

    pthread_t threads[NUM_THREADS];
    struct thread_parameter_t thread_data[NUM_THREADS];

    int thread_num = 0;

    // Start timing the validation
    clock_t start_time = clock();

    // Create threads for rows
    for (int i = 0; i < SIZE; ++i) {
        thread_data[thread_num] = (struct thread_parameter_t){thread_num, i, 0, puzzle_number, sudoku};
        pthread_create(&threads[thread_num], NULL, check_row, (void*)&thread_data[thread_num]);
        thread_num++;
    }

    // Create threads for columns
    for (int i = 0; i < SIZE; ++i) {
        thread_data[thread_num] = (struct thread_parameter_t){thread_num, 0, i, puzzle_number, sudoku};
        pthread_create(&threads[thread_num], NULL, check_column, (void*)&thread_data[thread_num]);
        thread_num++;
    }

    // Create threads for subgrids
    for (int row = 0; row < SIZE; row += 3) {
        for (int col = 0; col < SIZE; col += 3) {
            thread_data[thread_num] = (struct thread_parameter_t){thread_num, row, col, puzzle_number, sudoku};
            pthread_create(&threads[thread_num], NULL, check_subgrid, (void*)&thread_data[thread_num]);
            thread_num++;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Check final validation status
    bool is_valid = true;
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (result[i] == 0) {
            is_valid = false;
            break;
        }
    }
    // Calculate and display the time taken for the validation
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Display the final validation result
    if (is_valid) {
        printf("Puzzle %d is valid - Took %.6f secs\n", puzzle_number, time_taken);
    } else {
        printf("Puzzle %d is not valid - Took %.6f secs\n", puzzle_number, time_taken);
    }

    return NULL;
}

// Function to run all three puzzles simultaneously using the new method (11 threads per puzzle)
void run_multiple_puzzles_with_11_threads() {
    pthread_t puzzle_threads[3];
    int puzzle_numbers[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        pthread_create(&puzzle_threads[i], NULL, validate_puzzle_11_threads, &puzzle_numbers[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(puzzle_threads[i], NULL);
    }
}

void run_multiple_puzzles_simultaneously() {
    pthread_t puzzle_threads[3];
    int puzzle_numbers[3] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
        pthread_create(&puzzle_threads[i], NULL, validate_puzzle, &puzzle_numbers[i]);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(puzzle_threads[i], NULL);
    }
}

int main() {
    printf("Running validation with original 27-thread method:\n");
    run_multiple_puzzles_simultaneously();

    printf("\nRunning validation with new 11-thread method:\n");
    run_multiple_puzzles_with_11_threads();

    return 0;
}
