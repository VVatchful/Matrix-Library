/*

StockRecord:
    - date_string (or separate year, month, day fields)
    - open_price
    - high_price
    - low_price
    - close_price
    - volume

StockDataArray:
    - array of StockRecord pointers
    - current size
    - capacity (for dynamic resizing)
```

## Main Operations

### 1. File Reading Function
```
read_stock_csv(filename):
    attempt to open file
    if file doesn't exist:
        return error
    
    skip header line (Date,Open,High,Low,Close,Volume)
    
    initialize empty stock data array
    
    for each line in file:
        parse the line into tokens
        if parsing fails:
            log error, skip line
        
        validate each field (numbers in range, date format)
        if validation fails:
            log error, skip line
        
        create StockRecord from parsed data
        add to array (resize if needed)
    
    close file
    return stock data array
```

### 2. Line Parsing Helper
```
parse_csv_line(line):
    initialize empty token array
    
    for each character in line:
        handle comma delimiters
        handle quoted fields (if any)
        handle newline characters
        build tokens
    
    return array of tokens
```

### 3. Dynamic Array Management
```
add_stock_record(array, record):
    if array is full:
        double the capacity
        reallocate memory
        copy old data to new location
    
    insert record at current position
    increment size
```

### 4. Validation Functions
```
validate_price(value):
    check if greater than zero
    check if reasonable (not astronomical)
    return true/false

validate_volume(value):
    check if non-negative integer
    return true/false

validate_date(date_string):
    check format (YYYY-MM-DD)
    check month is 1-12
    check day is valid for month
    return true/false
```

### 5. Cleanup Function
```
free_stock_data(array):
    for each record in array:
        free record
    free array structure
```

## Testing Approach
```
create_test_csv():
    write file with header
    write 10-20 lines of fake data:
        - some valid records
        - some with missing fields
        - some with invalid prices (negative)
        - some with malformed dates
        - some with non-numeric data

test_reader():
    call read_stock_csv on test file
    verify correct number of valid records loaded
    verify invalid records were skipped
    print parsed data to verify correctness
    verify memory was properly allocated
    call cleanup function
```

## Error Handling Strategy
```
Throughout all functions:
    - check every malloc/realloc return
    - check file operations
    - validate all parsed data before storing
    - provide meaningful error messages
    - clean up partially allocated memory on errors
    - never crash, always gracefully handle bad input
```

## Usage Flow
```
main():
    filename = get from user or command line
    stock_data = read_stock_csv(filename)
    
    if successful:
        display summary (number of records, date range)
        optionally print first few records
        
        // later: pass to your RNN for training
        
        free_stock_data(stock_data)
    else:
        display error message

*/
