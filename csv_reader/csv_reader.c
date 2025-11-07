#include <asm-generic/errno-base.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

struct stockRecord {
    int year;
    int month;
    int day;
    float open_price;
    float high_price;
    float low_price;
    float close_price;
    long int volume;
    char original_data_string;
};

struct stockData {
    struct stockData* records;
    int size;
    int capacity;
};

#define INITIAL_CAPACITY = 100;
#define GROWTH_FACTOR = 2;

struct parserResults {
    bool success;
    char error_message;
    char tokens;
    int line_number;
};

int stock_data_csv() {
    const char *filename = "../CSV/all_stocks_combined.csv";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            printf("Couldn't find the file: %s\n", filename);
            return -1;
        } else if (errno == EACCES) {
            printf("Permission was denied for the file: %s\n", filename);
            return -1;
        } else {
            printf("Could not open file: %s\n", filename);  // Fixed typo!
            return -1;
        }
    }
    
    int stock_array = allocate_stock();
    if (stock_array == NULL) {
        fclose(file);
        printf("Memory allocation failed for stock array\n");
        return -1; 
    
    
    fclose(file);
    return 0;
}
