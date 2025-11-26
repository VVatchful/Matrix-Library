//
// Created by VVatchful on 11/25/2025.
//

#ifndef MATRIX_LIBRARY_STOCK_DATA_H
#define MATRIX_LIBRARY_STOCK_DATA_H

struct stockRecord {
    int year;
    int month;
    int day;
    float open_price;
    float high_price;
    float low_price;
    float close_price;
    long int volume;
};

struct stockData {
    char ticker[16];
    struct stockRecord* records;
    int size;
    int capacity;
};

struct allStocksData {
    struct stockData* stocks;
    int num_stocks;
    int capacity;
};

#define INITIAL_CAPACITY 100
#define GROWTH_FACTOR 2
#define MAX_LINE_LENGTH 50000

#endif //MATRIX_LIBRARY_STOCK_DATA_H