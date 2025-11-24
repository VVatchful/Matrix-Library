#ifdef NORMALIZATION_H
#define NORMALIZATION_H

#include <stdbool.h>

typedef enum {
    NORM_MINMAX,
    NORM_ZSCORE,
    NORM_NONE
} Normalization_type;

struct Normalization {
    Normalization_type type;
    float min_value;
    float max_value;
    float mean_value;
    float std_std_value;
    bool all_same_value;
    float constant_value;

    char feature_name[32];
};

struct stockNormalization {
    char ticker[16];
    struct Normalization params[5];
    int num_params;
    int num_records_used;
    int year_start, month_start, day_start, hour_start, min_start, sec_start;
    int year_end, month_end, day_end, hour_end, min_end, sec_end;
    int stock_index;
};

struct allStockNormalization {
    struct stockNormalization* normalizers;
    int num_stocks;
    int capacity;

    Normalization_type default_price_norm;
    Normalization_type stock_norm;
};

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
    int normalization_index;
};

struct allNormalizedStocks {
    struct normalizedStockData* stocks;
    int num_stocks;
    int capacity;
};

struct normalizedStockRecord {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;

    float open_price_norm;
    float high_price_norm;
    float low_price_norm;
    float close_price_norm;
    float volumes_norm;
};

struct normalizedStockDataRecords {
    char ticker[16];
    struct normalizedStockRecord* records;
    int size;
    int capacity;
    int normalization_index;
};

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