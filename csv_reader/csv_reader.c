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
    char *original_data_string;
};

struct stockData {
    struct stockRecord* records;
    int size;
    int capacity;
};

#define INITIAL_CAPACITY 100
#define GROWTH_FACTOR 2

struct parserResults {
    bool success;
    char error_message[256];
    char tokens[256];
    int line_number;
};

struct stockData* allocate_stock() {
    struct stockData* container = malloc(sizeof(struct stockData));
    if (container == NULL) {
        return NULL;
    }
    container->records = malloc(INITIAL_CAPACITY * sizeof(struct stockRecord));
    if (container->records == NULL) {
        free(container);
        return NULL;    
    }
    container->size = 0;
    container->capacity = INITIAL_CAPACITY;
    return container;
}

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
            printf("Could not open file: %s\n", filename);
            return -1;
        }
    }
    
    struct stockData* stock_array = allocate_stock();
    if (stock_array == NULL) {
        fclose(file);
        printf("Memory allocation failed for stock array\n");
        return -1;
    }
    
    // TODO: Read and parse the file
    // TODO: Add records to stock_array
    
    free(stock_array->records);
    free(stock_array);
    fclose(file);
    
    return 0;
}

