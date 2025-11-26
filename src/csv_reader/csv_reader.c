#include "stock_data.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

struct allStocksData* allocate_all_stocks(int num_stocks) {
    struct allStocksData* container = malloc(sizeof(struct allStocksData));
    if (container == NULL) {
        return NULL;
    }

    container->stocks = malloc(num_stocks * sizeof(struct stockData));
    if (container->stocks == NULL) {
        free(container);
        return NULL;
    }

    for (int i = 0; i < num_stocks; i++) {
        container->stocks[i].records = malloc(INITIAL_CAPACITY * sizeof(struct stockRecord));
        if (container->stocks[i].records == NULL) {
            for (int j = 0; j < i; j++) {
                free(container->stocks[j].records);
            }
            free(container->stocks);
            free(container);
            return NULL;
        }
        container->stocks[i].size = 0;
        container->stocks[i].capacity = INITIAL_CAPACITY;
        container->stocks[i].ticker[0] = '\0';
    }

    container->num_stocks = num_stocks;
    container->capacity = num_stocks;

    return container;
}

char* read_line(FILE* file) {
    int buffer_size = MAX_LINE_LENGTH;
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        return NULL;
    }
    int position = 0;
    int c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n' || c == '\r') {
            if (c == '\r') {
                c = fgetc(file);
                if (c != '\n' && c != EOF) {
                    ungetc(c, file);
                }
            }
            break;
        }
        buffer[position] = (char)c;
        position++;
        if (position >= buffer_size - 1) {
            buffer_size *= 2;
            char* new_buffer = realloc(buffer, buffer_size);
            if (new_buffer == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
    }
    if (position == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }
    buffer[position] = '\0';
    return buffer;
}

int count_unique_tickers(char* ticker_line) {
    if (ticker_line == NULL) return 0;

    size_t len = strlen(ticker_line) + 1;
    char* line_copy = malloc(len);
    if (line_copy == NULL) return 0;

    strcpy_s(line_copy, len, ticker_line);

    char tickers[100][16];
    int num_tickers = 0;

    char* next_token = NULL;
    char* token = strtok_s(line_copy, ",", &next_token);
    if (token && strcmp(token, "Ticker") == 0) {
        token = strtok_s(NULL, ",", &next_token);
    }

    while (token != NULL && num_tickers < 100) {
        int found = 0;
        for (int i = 0; i < num_tickers; i++) {
            if (strcmp(tickers[i], token) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            strncpy_s(tickers[num_tickers], 16, token, 15);
            num_tickers++;
        }
        token = strtok_s(NULL, ",", &next_token);
    }

    free(line_copy);
    return num_tickers;
}

void extract_tickers(char* ticker_line, struct allStocksData* all_stocks) {
    if (ticker_line == NULL || all_stocks == NULL) return;

    size_t len = strlen(ticker_line) + 1;
    char* line_copy = malloc(len);
    if (line_copy == NULL) return;

    strcpy_s(line_copy, len, ticker_line);

    char tickers[100][16];
    int num_tickers = 0;

    char* next_token = NULL;
    char* token = strtok_s(line_copy, ",", &next_token);
    if (token && strcmp(token, "Ticker") == 0) {
        token = strtok_s(NULL, ",", &next_token);
    }

    while (token != NULL && num_tickers < 100) {
        int found = -1;
        for (int i = 0; i < num_tickers; i++) {
            if (strcmp(tickers[i], token) == 0) {
                found = i;
                break;
            }
        }
        if (found == -1) {
            strncpy_s(tickers[num_tickers], 16, token, 15);
            strncpy_s(all_stocks->stocks[num_tickers].ticker, 16, token, 15);
            num_tickers++;
        }
        token = strtok_s(NULL, ",", &next_token);
    }

    free(line_copy);
}

bool add_record_to_stock(struct stockData* stock, struct stockRecord* record) {
    if (stock->size >= stock->capacity) {
        int new_capacity = stock->capacity * GROWTH_FACTOR;
        struct stockRecord* new_records = realloc(stock->records,
                                                   new_capacity * sizeof(struct stockRecord));
        if (new_records == NULL) {
            return false;
        }
        stock->records = new_records;
        stock->capacity = new_capacity;
    }

    stock->records[stock->size] = *record;
    stock->size++;
    return true;
}

bool parse_data_line(char* line, struct allStocksData* all_stocks, int line_number) {
    if (line == NULL || all_stocks == NULL) return false;

    size_t len = strlen(line) + 1;
    char* line_copy = malloc(len);
    if (line_copy == NULL) return false;

    strcpy_s(line_copy, len, line);

    char* tokens[1000];
    int token_count = 0;

    char* next_token = NULL;
    char* token = strtok_s(line_copy, ",", &next_token);
    while (token != NULL && token_count < 1000) {
        tokens[token_count] = token;
        token_count++;
        token = strtok_s(NULL, ",", &next_token);
    }

    if (token_count < 1) {
        free(line_copy);
        return false;
    }

    int year, month, day;
    if (sscanf_s(tokens[0], "%d-%d-%d", &year, &month, &day) != 3) {
        free(line_copy);
        return false;
    }

    int expected_tokens = 1 + (all_stocks->num_stocks * 5);
    if (token_count < expected_tokens) {
        fprintf(stderr, "Line %d: Expected %d tokens, got %d\n",
                line_number, expected_tokens, token_count);
        free(line_copy);
        return false;
    }

    int token_idx = 1;
    for (int stock_idx = 0; stock_idx < all_stocks->num_stocks; stock_idx++) {
        struct stockRecord record;
        record.year = year;
        record.month = month;
        record.day = day;

        if (token_idx + 4 >= token_count) {
            fprintf(stderr, "Line %d: Not enough data for stock %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
            free(line_copy);
            return false;
        }

        if (sscanf_s(tokens[token_idx], "%f", &record.open_price) != 1) {
            fprintf(stderr, "Line %d: Invalid open price for %s: '%s'\n",
                    line_number, all_stocks->stocks[stock_idx].ticker, tokens[token_idx]);
            token_idx += 5;
            continue;
        }
        token_idx++;

        if (sscanf_s(tokens[token_idx], "%f", &record.high_price) != 1) {
            fprintf(stderr, "Line %d: Invalid high price for %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
            token_idx += 4;
            continue;
        }
        token_idx++;

        if (sscanf_s(tokens[token_idx], "%f", &record.low_price) != 1) {
            fprintf(stderr, "Line %d: Invalid low price for %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
            token_idx += 3;
            continue;
        }
        token_idx++;

        if (sscanf_s(tokens[token_idx], "%f", &record.close_price) != 1) {
            fprintf(stderr, "Line %d: Invalid close price for %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
            token_idx += 2;
            continue;
        }
        token_idx++;

        if (sscanf_s(tokens[token_idx], "%ld", &record.volume) != 1) {
            fprintf(stderr, "Line %d: Invalid volume for %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
            token_idx++;
            continue;
        }
        token_idx++;

        if (!add_record_to_stock(&all_stocks->stocks[stock_idx], &record)) {
            fprintf(stderr, "Line %d: Failed to add record for %s\n",
                    line_number, all_stocks->stocks[stock_idx].ticker);
        }
    }

    free(line_copy);
    return true;
}

struct allStocksData* stock_data_csv(void) {
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

    char* ticker_line = read_line(file);
    if (ticker_line == NULL) {
        fclose(file);
        printf("Failed to read ticker line\n");
        return NULL;
    }

    int num_stocks = count_unique_tickers(ticker_line);
    printf("Found %d unique stocks in the file\n", num_stocks);

    if (num_stocks == 0) {
        free(ticker_line);
        fclose(file);
        printf("No stocks found in file\n");
        return NULL;
    }

    struct allStocksData* all_stocks = allocate_all_stocks(num_stocks);
    if (all_stocks == NULL) {
        free(ticker_line);
        fclose(file);
        printf("Memory allocation failed\n");
        return NULL;
    }

    extract_tickers(ticker_line, all_stocks);
    free(ticker_line);

    char* price_line = read_line(file);
    if (price_line == NULL) {
        for (int i = 0; i < all_stocks->num_stocks; i++) {
            free(all_stocks->stocks[i].records);
        }
        free(all_stocks->stocks);
        free(all_stocks);
        fclose(file);
        printf("Failed to read price line\n");
        return NULL;
    }
    free(price_line);

    char* date_header = read_line(file);
    if (date_header == NULL) {
        for (int i = 0; i < all_stocks->num_stocks; i++) {
            free(all_stocks->stocks[i].records);
        }
        free(all_stocks->stocks);
        free(all_stocks);
        fclose(file);
        printf("Failed to read date header\n");
        return NULL;
    }
    free(date_header);

    int line_number = 3;
    int total_lines = 0;

    while (!feof(file)) {
        char* line = read_line(file);

        if (line == NULL) {
            break;
        }

        line_number++;

        if (strlen(line) == 0 || line[0] == '#') {
            free(line);
            continue;
        }

        if (parse_data_line(line, all_stocks, line_number)) {
            total_lines++;
        }

        free(line);
    }

    fclose(file);

    printf("\nSuccessfully parsed %d data lines\n", total_lines);
    printf("\nStock summary:\n");
    for (int i = 0; i < all_stocks->num_stocks; i++) {
        printf("  %-10s: %d records\n",
               all_stocks->stocks[i].ticker,
               all_stocks->stocks[i].size);
    }

    return all_stocks;
}

int main(void) {
    struct allStocksData* data = stock_data_csv();

    if (data == NULL) {
        printf("Failed to load stock data\n");
        return 1;
    }

    printf("\n=== Sample Data ===\n");
    for (int i = 0; i < data->num_stocks && i < 3; i++) {
        if (data->stocks[i].size > 0) {
            printf("\n%s - First record:\n", data->stocks[i].ticker);
            printf("  Date: %d-%02d-%02d\n",
                   data->stocks[i].records[0].year,
                   data->stocks[i].records[0].month,
                   data->stocks[i].records[0].day);
            printf("  Open: %.2f, Close: %.2f\n",
                   data->stocks[i].records[0].open_price,
                   data->stocks[i].records[0].close_price);
            printf("  Volume: %ld\n", data->stocks[i].records[0].volume);
        }
    }

    for (int i = 0; i < data->num_stocks; i++) {
        free(data->stocks[i].records);
    }
    free(data->stocks);
    free(data);

    return 0;
}
