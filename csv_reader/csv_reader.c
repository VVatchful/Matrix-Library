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
    FILE *file = fopen("../CSV/all_stocks_combined.csv", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening CSV file: %s : %s/n", filename, strerror(errno));
        fclose(file);
        return 0;
    } else {
        perror("Persmission was denied or could not be opened. Please check your file perms:");
        return 0;
    }
    
}
