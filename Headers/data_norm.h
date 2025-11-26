#pragma once
#ifdef NORMALIZATION_H
#define NORMALIZATION_H

#include <stdbool.h>

// Defines the normalization method that will be encompassed with later features

typedef enum {
    NORM_MINMAX,
    NORM_ZSCORE,
    NORM_NONE
} Normalization_type;

// Params that are needed to normalize and denormalize a given feature

struct Normalization {
    Normalization_type type;
    float min_value;
    float max_value;
    float mean_value;
    float std_std_value;
    bool all_same_value;
    float constant_value;

    char feature_name[32]; // debugging variable
};

// Contains the params for each individual stock

struct stockNormalization {
    char ticker[16];
    struct Normalization params[5];
    int num_features;

    // Meta data
    int num_records_used;
    int year_start, month_start, day_start, hour_start, min_start, sec_start;
    int year_end, month_end, day_end, hour_end, min_end, sec_end;

    // Look up to original data set
    int stock_index;
};
//Holds normalizers for all given stocks within the csv_reader
struct allStockNormalization {
    struct stockNormalization* normalizers;
    int num_stocks;
    int capacity;

    Normalization_type default_price_norm;
    Normalization_type stock_norm;
};
// Keeps data in separate arrays, aiming to increase batch operations speed
struct normalizedStockData {
    char ticker[16];
    float* open_prices_norm;
    float* high_prices_norm;
    float* low_prices_norm;
    float* close_prices_norm;
    float* volumes_norm;

    int* years;
    int* months;
    int* days;
    int* hours;
    int* mins;
    int* secs;

    int size;
    int capacity;
    int normalization_index; // Links to the normalizer for specific stock
};

struct allNormalizedStocks {
    struct normalizedStockData* stocks;
    int num_stocks;
    int capacity;
};

// helper function
struct featureArray {
    float* values;
    int size;
    int capacity;
};

#define NUM_STOCK_FEATURES 5        // open, high, low, close, volume
#define EPSILON 1e-8f               // Small value to prevent division by zero
#define OUTLIER_THRESHOLD 4.0f      // Z-score threshold for outlier detection

#define FEATURE_OPEN   0
#define FEATURE_HIGH   1
#define FEATURE_LOW    2
#define FEATURE_CLOSE  3
#define FEATURE_VOLUME 4

#endif