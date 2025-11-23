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
    char* tokens[10];
    int line_number;
    int token_count;
};

struct validationResults {
    bool success;
    char error_message[256];
    struct stockRecord* records;
};

struct addResults {
    bool success;
    char error_message[256];
};

struct parserResults parse_csv_line(char* line, int line_number);
struct validationResults validate_csv_line(char** tokens, int token_count, int line_number);
struct addResults add_stock_record(struct stockData* array, struct stockRecord* record);

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

char* read_line(FILE* file) {
    int buffer_size = 1024;
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        return NULL;
    }
    int position = 0;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            break;
        }
        buffer[position] = (char)c;
        position++;
        if (position >= buffer_size - 1) {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);
            if (buffer == NULL) {
                return NULL;
            }
        }
    }
    if (position == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }
    buffer[position] = '\0';
    return buffer;
}

struct stockData* stock_data_csv(void) {
    const char *filename = "../CSV/all_stocks_combined.csv";
    FILE *file = NULL;
    errno_t err = fopen_s(&file, filename, "r");

    if (err != 0 || file == NULL) {
        if (err == ENOENT) {
            printf("Couldn't find the file: %s\n", filename);
        } else if (err == EACCES) {
            printf("Permission was denied for the file: %s\n", filename);
        } else {
            printf("Could not open file: %s\n", filename);
        }
        return NULL;
    }

    struct stockData* stock_array = allocate_stock();
    if (stock_array == NULL) {
        fclose(file);
        printf("Memory allocation failed for stock array\n");
        return NULL;
    }

    char* header_line = read_line(file);
    if (header_line == NULL) {
        free(stock_array->records);
        free(stock_array);
        fclose(file);
        printf("File was empty or header is missing\n");
        return NULL;
    }

    char* expect_header = "Date,Open,High,Low,Close,Volume";
    if (strcmp(header_line, expect_header) != 0) {
        fprintf(stderr, "Header line mismatch detected. Expected: %s Got: %s\n",
                expect_header, header_line);
    }

    free(header_line);

    int line_number = 1;
    int valid_records = 0;
    int invalid_records = 0;

    while (!feof(file)) {
        char* line = read_line(file);

        if (line == NULL) {
            break;
        }

        line_number++;

        if (line[0] == '#') {
            free(line);
            continue;
        }

        struct parserResults parse_result = parse_csv_line(line, line_number);
        if (!parse_result.success) {
            fprintf(stderr, "%s\n", parse_result.error_message);
            invalid_records++;
            free(line);
            continue;
        }

        if (parse_result.token_count != 6) {
            fprintf(stderr, "Line %d: Expected 6 tokens but found %d\n",
                    line_number, parse_result.token_count);
            invalid_records++;
            for (int i = 0; i < parse_result.token_count; i++) {
                free(parse_result.tokens[i]);
            }
            free(line);
            continue;
        }

        struct validationResults validate_results = validate_csv_line(parse_result.tokens, parse_result.token_count, line_number);

        for (int i = 0; i < parse_result.token_count; i++) {
            free(parse_result.tokens[i]);
        }

        if (!validate_results.success) {
            fprintf(stderr, "%s\n", validate_results.error_message);
            invalid_records++;
            free(line);
            continue;
        }

        struct addResults add_result = add_stock_record(stock_array, validate_results.records);
        if (!add_result.success) {
            fprintf(stderr, "Failed to add record at line %d: %s\n",
                    line_number, add_result.error_message);

            free(validate_results.records);
            free(line);
            free(stock_array->records);
            free(stock_array);
            fclose(file);
            return NULL;
        }

        free(validate_results.records);
        valid_records++;
        free(line);
    }

    fprintf(stdout, "Successfully loaded %d stock records\n", valid_records);
    if (invalid_records > 0) {
        fprintf(stdout, "Skipped %d invalid records\n", invalid_records);
    }

    if (valid_records == 0) {
        free(stock_array->records);
        free(stock_array);
        fclose(file);
        fprintf(stderr, "No valid records found in file\n");
        return NULL;
    }

    fclose(file);

    return stock_array;
}

struct parserResults parse_csv_line(char* line, int line_number) {
    struct parserResults result;
    result.success = false;
    result.token_count = 0;
    result.line_number = line_number;

    if (line == NULL || strlen(line) == 0) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Empty line", line_number);
        return result;
    }

    char* line_copy = strdup(line);
    if (line_copy == NULL) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Memory allocation failed", line_number);
        return result;
    }

    char* token = strtok(line_copy, ",");
    while (token != NULL && result.token_count < 10) {
        result.tokens[result.token_count] = strdup(token);
        if (result.tokens[result.token_count] == NULL) {
            snprintf(result.error_message, sizeof(result.error_message),
                    "Line %d: Memory allocation failed for token", line_number);
            free(line_copy);
            for (int i = 0; i < result.token_count; i++) {
                free(result.tokens[i]);
            }
            return result;
        }
        result.token_count++;
        token = strtok(NULL, ",");
    }

    free(line_copy);
    result.success = true;
    return result;
}

struct validationResults validate_csv_line(char** tokens, int token_count, int line_number) {
    struct validationResults result;
    result.success = false;
    result.records = NULL;

    if (token_count != 6) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Expected 6 fields but got %d", line_number, token_count);
        return result;
    }

    struct stockRecord* record = malloc(sizeof(struct stockRecord));
    if (record == NULL) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Memory allocation failed", line_number);
        return result;
    }

    if (sscanf_s(tokens[0], "%d-%d-%d", &record->year, &record->month, &record->day) != 3) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid date format '%s'", line_number, tokens[0]);
        free(record);
        return result;
    }

    if (sscanf_s(tokens[1], "%f", &record->open_price) != 1) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid open price '%s'", line_number, tokens[1]);
        free(record);
        return result;
    }

    if (sscanf_s(tokens[2], "%f", &record->high_price) != 1) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid high price '%s'", line_number, tokens[2]);
        free(record);
        return result;
    }

    if (sscanf_s(tokens[3], "%f", &record->low_price) != 1) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid low price '%s'", line_number, tokens[3]);
        free(record);
        return result;
    }

    if (sscanf_s(tokens[4], "%f", &record->close_price) != 1) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid close price '%s'", line_number, tokens[4]);
        free(record);
        return result;
    }

    if (sscanf_s(tokens[5], "%ld", &record->volume) != 1) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Line %d: Invalid volume '%s'", line_number, tokens[5]);
        free(record);
        return result;
    }

    record->original_data_string = NULL;

    result.success = true;
    result.records = record;
    return result;
}

struct addResults add_stock_record(struct stockData* array, struct stockRecord* record) {
    struct addResults result;
    result.success = false;

    if (array == NULL || record == NULL) {
        snprintf(result.error_message, sizeof(result.error_message),
                "Invalid array or record pointer");
        return result;
    }

    if (array->size >= array->capacity) {
        int new_capacity = array->capacity * GROWTH_FACTOR;
        struct stockRecord* new_records = realloc(array->records,
                                                   new_capacity * sizeof(struct stockRecord));
        if (new_records == NULL) {
            snprintf(result.error_message, sizeof(result.error_message),
                    "Failed to resize array");
            return result;
        }
        array->records = new_records;
        array->capacity = new_capacity;
    }

    array->records[array->size] = *record;
    array->size++;

    result.success = true;
    return result;
}

int main(void) {
    struct stockData* data = stock_data_csv();

    if (data == NULL) {
        printf("Failed to load stock data\n");
        return 1;
    }

    printf("\nFirst record:\n");
    printf("Date: %d-%02d-%02d\n", data->records[0].year, data->records[0].month, data->records[0].day);
    printf("Close: %.2f\n", data->records[0].close_price);

    free(data->records);
    free(data);

    return 0;
}