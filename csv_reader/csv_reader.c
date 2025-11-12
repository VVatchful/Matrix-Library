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

    stock_array->size = 0;
    stock_array->capacity = INITIAL_CAPACITY;
    stock_array->records = malloc(INITIAL_CAPACITY * sizeof(struct stockRecord));
    if (stock_array->records == NULL) {
    free(stock_array->records);
    fclose(file);}
    return printf("Memory allocation failed for stock array\n");

    char header_line = read_line(file);
    if (header_line == NULL) {
    free(stock_array->records);
    fclose(file);
    return printf("File was empty or header is missing\n");
    }

    expect_header = "Date Open, High, Low, Close, Volume";
    if (header_line != expect_header) {
        fprintf(stderr, "Header line mismatch detected. Format Undetected: %s + %s\n", expect_header, header_line);
    }
    line_number = 1;
    valid_records = 0;
    invalid_records = 0;

    while (!feof(file)) {
        line = read_line(file);
        line_number++;

        if (line[0] == '#') {
            continue;
        }

        parse_result = parse_csv_line(line, line_number);
        if (!parse_result.success) {
            fprintf(stdout, "%s\n", parse_result.error_message);
            invalid_records++;
            continue;
        }

        if (parse_result.tokens.length != 6) {
            fprintf("Line %d: Expected 6 tokens but found %d\n", parse_result.tokens.length, 6);
            invalid_records++;
            continue;
        }

        validate_results = validate_csv_line(parse_result.tokens, line_number);
        if (!validate_results.success) {
            fprintf(stdout, "%s\n", validate_results.error_message);
            invalid_records++;
            continue;
        }

        add_result = add_stock_record(stock_array, validate_results.records);
        if (!add_result.success) {
            free(validate_results.records);
            free(stock_array->records);
            fclose(file);
            return perror("Failed to add record at line: %d : %s", line_number, add_result.error_message);
        }

        valid_records++;
    }
    
    fprintf("Sucessfully loaded %d stock records\n", valid_records);
    if (invalid_records > 0) {
        fprintf("Skipped %d stock records\n", invalid_records);
    }

    if (valid_records > 0) {
        free(stock_array->records);
        return perror("No valid records found in file");
    }

    free(stock_array->records);
    free(stock_array);
    fclose(file);
    
    return stock_array->records;
}

