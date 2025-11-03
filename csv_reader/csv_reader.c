#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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


