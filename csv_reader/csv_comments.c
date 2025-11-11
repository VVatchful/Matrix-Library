/*

StockRecord:
    integer: year
    integer: month (1-12)
    integer: day (1-31)
    float: open_price
    float: high_price
    float: low_price
    float: close_price
    long integer: volume
    string: original_date_string (optional, for debugging)

StockDataArray:
    StockRecord**: records (array of pointers)
    integer: size (current number of records)
    integer: capacity (total allocated space)
    constant integer: INITIAL_CAPACITY = 100
    constant integer: GROWTH_FACTOR = 2

ParseResult:
    boolean: success
    string: error_message
    string[]: tokens (if successful)
    integer: line_number (for error reporting)
```

## Main Operations

### 1. File Reading Function
```
read_stock_csv(filename):
    // File opening with detailed error handling
    file_handle = attempt_file_open(filename, READ_MODE)
    if file_handle is null:
        if file does not exist:
            return error("File not found: " + filename)
        else if no read permission:
            return error("Permission denied: " + filename)
        else:
            return error("Cannot open file: " + filename)
    
    // Initialize array
    stock_array = allocate_stock_array()
    if stock_array is null:
        close file_handle
        return error("Memory allocation failed for stock array")
    
    initialize stock_array.size = 0
    initialize stock_array.capacity = INITIAL_CAPACITY
    stock_array.records = allocate memory for INITIAL_CAPACITY pointers
    if stock_array.records is null:
        free stock_array
        close file_handle
        return error("Memory allocation failed for records array")
    
    // Read and skip header
    header_line = read_line(file_handle)
    if header_line is null or empty:
        cleanup_and_close(stock_array, file_handle)
        return error("File is empty or header missing")
    
    expected_header = "Date,Open,High,Low,Close,Volume"
    if not matches_header(header_line, expected_header):
        log_warning("Header format unexpected: " + header_line)
        // Continue anyway, but warn user
    
    // Process data lines
    line_number = 1  // header is line 0
    valid_records_count = 0
    invalid_records_count = 0
    
    while not end_of_file(file_handle):
        line = read_line(file_handle)
        line_number++
        
        if line is null or empty:
            continue  // skip blank lines
        
        // Parse the line
        parse_result = parse_csv_line(line, line_number)
        
        if not parse_result.success:
            log_error("Line " + line_number + ": " + parse_result.error_message)
            invalid_records_count++
            continue
        
        // Validate token count
        if parse_result.tokens.length != 6:
            log_error("Line " + line_number + ": Expected 6 fields, got " + 
                     parse_result.tokens.length)
            invalid_records_count++
            continue
        
        // Parse and validate each field
        validation_result = parse_and_validate_record(parse_result.tokens, line_number)
        
        if not validation_result.success:
            log_error("Line " + line_number + ": " + validation_result.error_message)
            invalid_records_count++
            continue
        
        // Add valid record to array
        add_result = add_stock_record(stock_array, validation_result.record)
        
        if not add_result.success:
            free(validation_result.record)
            cleanup_and_close(stock_array, file_handle)
            return error("Failed to add record at line " + line_number + 
                        ": " + add_result.error_message)
        
        valid_records_count++
    
    close file_handle
    
    // Final report
    log_info("Successfully loaded " + valid_records_count + " records")
    if invalid_records_count > 0:
        log_warning("Skipped " + invalid_records_count + " invalid records")
    
    if valid_records_count == 0:
        free_stock_data(stock_array)
        return error("No valid records found in file")
    
    // Optionally shrink array to actual size to save memory
    shrink_to_fit(stock_array)
    
    return stock_array
```

### 2. Line Parsing Helper
```
parse_csv_line(line, line_number):
    result = new ParseResult
    result.line_number = line_number
    result.tokens = new array
    
    current_token = empty string
    inside_quotes = false
    position = 0
    
    for each character c at position in line:
        if c is quote character ("):
            if inside_quotes:
                // Check if it's an escaped quote
                if next_character is also quote:
                    append quote to current_token
                    skip next character
                else:
                    inside_quotes = false
            else:
                inside_quotes = true
        
        else if c is comma:
            if inside_quotes:
                // Comma is part of the data
                append c to current_token
            else:
                // End of field
                trim_whitespace(current_token)
                append current_token to result.tokens
                current_token = empty string
        
        else if c is newline or carriage return:
            if inside_quotes:
                result.success = false
                result.error_message = "Unclosed quote at position " + position
                return result
            else:
                // End of line, ignore
                break
        
        else:
            append c to current_token
        
        position++
    
    // Check for unclosed quotes
    if inside_quotes:
        result.success = false
        result.error_message = "Unclosed quote in line"
        return result
    
    // Add final token
    trim_whitespace(current_token)
    append current_token to result.tokens
    
    result.success = true
    return result
```

### 3. Parse and Validate Record
```
parse_and_validate_record(tokens, line_number):
    result = new ValidationResult
    record = allocate new StockRecord
    
    if record is null:
        result.success = false
        result.error_message = "Memory allocation failed"
        return result
    
    // Parse date (token[0])
    date_result = parse_date(tokens[0])
    if not date_result.success:
        free(record)
        result.success = false
        result.error_message = "Invalid date format: " + tokens[0] + 
                              " (" + date_result.error_message + ")"
        return result
    
    record.year = date_result.year
    record.month = date_result.month
    record.day = date_result.day
    
    // Parse open price (token[1])
    open_result = parse_float(tokens[1])
    if not open_result.success or not validate_price(open_result.value, "open"):
        free(record)
        result.success = false
        result.error_message = "Invalid open price: " + tokens[1]
        return result
    record.open_price = open_result.value
    
    // Parse high price (token[2])
    high_result = parse_float(tokens[2])
    if not high_result.success or not validate_price(high_result.value, "high"):
        free(record)
        result.success = false
        result.error_message = "Invalid high price: " + tokens[2]
        return result
    record.high_price = high_result.value
    
    // Parse low price (token[3])
    low_result = parse_float(tokens[3])
    if not low_result.success or not validate_price(low_result.value, "low"):
        free(record)
        result.success = false
        result.error_message = "Invalid low price: " + tokens[3]
        return result
    record.low_price = low_result.value
    
    // Parse close price (token[4])
    close_result = parse_float(tokens[4])
    if not close_result.success or not validate_price(close_result.value, "close"):
        free(record)
        result.success = false
        result.error_message = "Invalid close price: " + tokens[4]
        return result
    record.close_price = close_result.value
    
    // Parse volume (token[5])
    volume_result = parse_long_integer(tokens[5])
    if not volume_result.success or not validate_volume(volume_result.value):
        free(record)
        result.success = false
        result.error_message = "Invalid volume: " + tokens[5]
        return result
    record.volume = volume_result.value
    
    // Cross-field validation
    if not validate_price_relationships(record):
        error_msg = "Price relationships invalid: high=" + record.high_price +
                   " must be >= open=" + record.open_price +
                   ", low=" + record.low_price +
                   ", close=" + record.close_price
        free(record)
        result.success = false
        result.error_message = error_msg
        return result
    
    result.success = true
    result.record = record
    return result
```

### 4. Dynamic Array Management
```
add_stock_record(array, record):
    result = new AddResult
    
    // Check if resize needed
    if array.size >= array.capacity:
        new_capacity = array.capacity * GROWTH_FACTOR
        
        // Check for overflow
        if new_capacity < array.capacity:
            result.success = false
            result.error_message = "Array size overflow"
            return result
        
        // Reallocate
        new_records = reallocate(array.records, new_capacity * sizeof(StockRecord*))
        
        if new_records is null:
            result.success = false
            result.error_message = "Memory reallocation failed (tried to allocate " +
                                  new_capacity + " slots)"
            return result
        
        array.records = new_records
        array.capacity = new_capacity
        
        log_debug("Resized array from " + (new_capacity / GROWTH_FACTOR) +
                 " to " + new_capacity + " capacity")
    
    // Add record
    array.records[array.size] = record
    array.size++
    
    result.success = true
    return result

shrink_to_fit(array):
    if array.size < array.capacity:
        new_records = reallocate(array.records, array.size * sizeof(StockRecord*))
        if new_records is not null:
            array.records = new_records
            array.capacity = array.size
            log_debug("Shrunk array to " + array.size + " capacity")
```

### 5. Validation Functions
```
parse_date(date_string):
    result = new DateParseResult
    
    // Expected format: YYYY-MM-DD
    if length of date_string != 10:
        result.success = false
        result.error_message = "Expected format YYYY-MM-DD (length 10)"
        return result
    
    if date_string[4] != '-' or date_string[7] != '-':
        result.success = false
        result.error_message = "Expected dashes at positions 4 and 7"
        return result
    
    year_str = substring(date_string, 0, 4)
    month_str = substring(date_string, 5, 2)
    day_str = substring(date_string, 8, 2)
    
    year = parse_integer(year_str)
    month = parse_integer(month_str)
    day = parse_integer(day_str)
    
    if year < 1900 or year > 2100:
        result.success = false
        result.error_message = "Year out of range (1900-2100): " + year
        return result
    
    if month < 1 or month > 12:
        result.success = false
        result.error_message = "Month out of range (1-12): " + month
        return result
    
    days_in_month = get_days_in_month(month, year)
    if day < 1 or day > days_in_month:
        result.success = false
        result.error_message = "Day " + day + " invalid for month " + month
        return result
    
    result.success = true
    result.year = year
    result.month = month
    result.day = day
    return result

get_days_in_month(month, year):
    days_per_month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
    
    if month == 2 and is_leap_year(year):
        return 29
    else:
        return days_per_month[month - 1]

is_leap_year(year):
    if year divisible by 400:
        return true
    if year divisible by 100:
        return false
    if year divisible by 4:
        return true
    return false

validate_price(value, field_name):
    constant MIN_PRICE = 0.01
    constant MAX_PRICE = 1000000.00  // $1 million per share
    
    if value <= 0:
        log_error(field_name + " price must be positive: " + value)
        return false
    
    if value < MIN_PRICE:
        log_warning(field_name + " price unusually low: " + value)
        // Could still accept it, just warn
    
    if value > MAX_PRICE:
        log_error(field_name + " price exceeds maximum: " + value)
        return false
    
    if is_NaN(value) or is_infinite(value):
        log_error(field_name + " price is invalid number")
        return false
    
    return true

validate_volume(value):
    constant MAX_VOLUME = 10000000000  // 10 billion shares
    
    if value < 0:
        log_error("Volume cannot be negative: " + value)
        return false
    
    if value == 0:
        log_warning("Volume is zero (market closed or no trades?)")
        // Accept it but warn
    
    if value > MAX_VOLUME:
        log_error("Volume exceeds maximum: " + value)
        return false
    
    return true

validate_price_relationships(record):
    // High should be highest
    if record.high_price < record.open_price or
       record.high_price < record.close_price or
       record.high_price < record.low_price:
        return false
    
    // Low should be lowest
    if record.low_price > record.open_price or
       record.low_price > record.close_price or
       record.low_price > record.high_price:
        return false
    
    // Allow for some floating point tolerance
    tolerance = 0.01
    if record.high_price - record.low_price < -tolerance:
        return false
    
    return true

parse_float(string):
    result = new ParseFloatResult
    
    if string is empty:
        result.success = false
        return result
    
    // Try to convert string to float
    value = convert_to_float(string)
    
    if conversion failed:
        result.success = false
        return result
    
    result.success = true
    result.value = value
    return result

parse_long_integer(string):
    result = new ParseLongResult
    
    if string is empty:
        result.success = false
        return result
    
    // Try to convert string to long integer
    value = convert_to_long(string)
    
    if conversion failed:
        result.success = false
        return result
    
    result.success = true
    result.value = value
    return result
```

### 6. Cleanup Function
```
free_stock_data(array):
    if array is null:
        return
    
    if array.records is not null:
        // Free each individual record
        for i from 0 to array.size - 1:
            if array.records[i] is not null:
                free(array.records[i])
                array.records[i] = null
        
        // Free the array of pointers
        free(array.records)
        array.records = null
    
    // Free the array structure itself
    free(array)

cleanup_and_close(array, file_handle):
    if array is not null:
        free_stock_data(array)
    if file_handle is not null:
        close(file_handle)
```

## Testing Approach
```
create_test_csv(filename):
    file = create_file(filename)
    
    write_line(file, "Date,Open,High,Low,Close,Volume")
    
    // Valid records
    write_line(file, "2024-01-02,150.25,155.50,149.75,154.00,1000000")
    write_line(file, "2024-01-03,154.00,156.00,153.00,155.50,1200000")
    write_line(file, "2024-01-04,155.50,157.25,154.50,156.75,950000")
    
    // Edge cases - valid but unusual
    write_line(file, "2024-02-29,100.00,100.01,99.99,100.00,0")  // leap year, zero volume
    write_line(file, "2024-01-05,0.01,0.02,0.01,0.01,100")  // penny stock
    
    // Invalid records - missing fields
    write_line(file, "2024-01-06,160.00,162.00,159.00")  // missing close and volume
    write_line(file, "2024-01-07")  // only date
    
    // Invalid records - bad prices
    write_line(file, "2024-01-08,-150.00,155.00,149.00,154.00,1000000")  // negative open
    write_line(file, "2024-01-09,150.00,abc,149.00,154.00,1000000")  // non-numeric high
    write_line(file, "2024-01-10,150.00,140.00,149.00,154.00,1000000")  // high < low
    
    // Invalid records - bad dates
    write_line(file, "2024-13-01,150.00,155.00,149.00,154.00,1000000")  // month 13
    write_line(file, "2024-02-30,150.00,155.00,149.00,154.00,1000000")  // Feb 30
    write_line(file, "24-01-15,150.00,155.00,149.00,154.00,1000000")  // wrong format
    write_line(file, "not-a-date,150.00,155.00,149.00,154.00,1000000")
    
    // Invalid records - bad volume
    write_line(file, "2024-01-16,150.00,155.00,149.00,154.00,-1000")  // negative volume
    write_line(file, "2024-01-17,150.00,155.00,149.00,154.00,xyz")  // non-numeric volume
    
    // Empty line (should be skipped)
    write_line(file, "")
    
    // More valid records
    write_line(file, "2024-01-18,157.00,158.50,156.00,158.00,1100000")
    
    // Quoted field (if CSV supports it)
    write_line(file, "2024-01-19,\"158.00\",159.00,157.50,158.75,1050000")
    
    close(file)

test_reader():
    test_filename = "test_stock_data.csv"
    
    print("Creating test CSV file...")
    create_test_csv(test_filename)
    
    print("Reading and parsing CSV file...")
    stock_data = read_stock_csv(test_filename)
    
    if stock_data is error:
        print("ERROR: Failed to read file: " + stock_data.error_message)
        return FAIL
    
    print("Successfully loaded " + stock_data.size + " records")
    
    // Verify expected count (should be ~5-6 valid records out of 19 lines)
    expected_valid = 6
    if stock_data.size < expected_valid - 1 or stock_data.size > expected_valid + 1:
        print("WARNING: Expected approximately " + expected_valid + 
              " valid records, got " + stock_data.size)
    
    print("\nFirst 5 records:")
    for i from 0 to minimum(4, stock_data.size - 1):
        record = stock_data.records[i]
        print("  " + record.year + "-" + 
              pad_zero(record.month) + "-" + 
              pad_zero(record.day) + " | " +
              "O: " + format_price(record.open_price) + " " +
              "H: " + format_price(record.high_price) + " " +
              "L: " + format_price(record.low_price) + " " +
              "C: " + format_price(record.close_price) + " " +
              "V: " + format_volume(record.volume))
    
    // Verify date ordering (if we expect chronological)
    if stock_data.size > 1:
        for i from 0 to stock_data.size - 2:
            date1 = date_to_comparable(stock_data.records[i])
            date2 = date_to_comparable(stock_data.records[i + 1])
            if date1 > date2:
                print("WARNING: Records not in chronological order at index " + i)
    
    // Memory check - verify no null pointers
    for i from 0 to stock_data.size - 1:
        if stock_data.records[i] is null:
            print("ERROR: Null record at index " + i)
            free_stock_data(stock_data)
            return FAIL
    
    print("\nFreeing memory...")
    free_stock_data(stock_data)
    
    print("Test completed successfully!")
    return PASS

date_to_comparable(record):
    return record.year * 10000 + record.month * 100 + record.day
```

## Usage Flow
```
main(arguments):
    print("Stock Data CSV Reader v1.0")
    print("===========================\n")
    
    // Get filename
    filename = null
    if arguments.length > 1:
        filename = arguments[1]
    else:
        print("Enter CSV filename: ")
        filename = read_user_input()
    
    if filename is empty:
        print("ERROR: No filename provided")
        return EXIT_FAILURE
    
    // Read and parse
    print("Loading data from: " + filename)
    start_time = current_time()
    
    stock_data = read_stock_csv(filename)
    
    elapsed_time = current_time() - start_time
    
    if stock_data is error:
        print("\nERROR: " + stock_data.error_message)
        print("Please check the file and try again.")
        return EXIT_FAILURE
    
    // Display summary
    print("\n=== Summary ===")
    print("Total records: " + stock_data.size)
    print("Load time: " + elapsed_time + " seconds")
    
    if stock_data.size > 0:
        first_record = stock_data.records[0]
        last_record = stock_data.records[stock_data.size - 1]
        
        print("Date range: " + format_date(first_record) + 
              " to " + format_date(last_record))
        
        // Calculate statistics
        stats = calculate_statistics(stock_data)
        print("Average close price: $" + format_price(stats.avg_close))
        print("Average volume: " + format_volume(stats.avg_volume))
        print("Price range: $" + format_price(stats.min_price) +
              " - $" + format_price(stats.max_price))
    
    // Display sample records
    print("\n=== Sample Records (first 10) ===")
    display_records(stock_data, 0, minimum(9, stock_data.size - 1))
    
    // Optional: Interactive menu
    while true:
        print("\nOptions:")
        print("  1. Display all records")
        print("  2. Display date range")
        print("  3. Calculate statistics")
        print("  4. Export to different format")
        print("  5. Run RNN training (placeholder)")
        print("  0. Exit")
        
        choice = read_integer_input()
        
        if choice == 0:
            break
        else if choice == 1:
            display_records(stock_data, 0, stock_data.size - 1)
        else if choice == 2:
            start_date = read_date_input("Start date (YYYY-MM-DD): ")
            end_date = read_date_input("End date (YYYY-MM-DD): ")
            filtered = filter_by_date_range(stock_data, start_date, end_date)
            display_records(filtered, 0, filtered.size - 1)
            free_stock_data(filtered)
        else if choice == 3:
            stats = calculate_statistics(stock_data)
            display_detailed_statistics(stats)
        else if choice == 4:
            print("Export not yet implemented")
        else if choice == 5:
            print("RNN training interface not yet implemented")
            print("(Pass stock_data to your neural network module here)")
        else:
            print("Invalid choice")
    
    // Cleanup
    print("\nCleaning up...")
    free_stock_data(stock_data)
    
    print("Done!")
    return EXIT_SUCCESS

calculate_statistics(stock_data):
    if stock_data.size == 0:
        return null
    
    stats = new Statistics
    stats.min_price = infinity
    stats.max_price = -infinity
    sum_close = 0.0
    sum_volume = 0
    
    for i from 0 to stock_data.size - 1:
        record = stock_data.records[i]
        
        // Update min/max across all price fields
        stats.min_price = minimum(stats.min_price, record.low_price)
        stats.max_price = maximum(stats.max_price, record.high_price)
        
        // Sum for averages
        sum_close += record.close_price
        sum_volume += record.volume
    
    stats.avg_close = sum_close / stock_data.size
    stats.avg_volume = sum_volume / stock_data.size
    
    return stats

display_records(stock_data, start_index, end_index):
    print("Date       | Open    | High    | Low     | Close   | Volume")
    print("-----------|---------|---------|---------|---------|------------")
    
    for i from start_index to end_index:
        record = stock_data.records[i]
        print(format_date(record) + " | " +
              pad_right(format_price(record.open_price), 7) + " | " +
              pad_right(format_price(record.high_price), 7) + " | " +
              pad_right(format_price(record.low_price), 7) + " | " +
              pad_right(format_price(record.close_price), 7) + " | " +
              format_volume(record.volume))

*/
