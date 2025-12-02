/*

NormalizationParams:
    - normalization_type (enum: MIN_MAX, Z_SCORE)
    - min_value
    - max_value
    - mean_value
    - std_dev_value
    - feature_name (optional identifier)

DataNormalizer:
    - array of NormalizationParams (one per feature)
    - number_of_features
    - original_data_reference (optional)
```

## Statistical Helper Functions
```
find_min(data_array, size):
    min_value = first element
    for each element in array:
        if element < min_value:
            update min_value
    return min_value

find_max(data_array, size):
    max_value = first element
    for each element in array:
        if element > max_value:
            update max_value
    return max_value

calculate_mean(data_array, size):
    sum = 0
    for each element:
        sum += element
    return sum / size

calculate_std_dev(data_array, size, mean):
    sum_squared_differences = 0
    for each element:
        difference = element - mean
        sum_squared_differences += difference * difference
    variance = sum_squared_differences / size
    return square_root(variance)
```

## Min-Max Normalization
```
minmax_normalize(data_array, size):
    min_val = find_min(data_array, size)
    max_val = find_max(data_array, size)
    
    // Handle edge case where all values are the same
    if min_val == max_val:
        return array of zeros or ones
    
    create normalized_array
    
    for each value in data_array:
        normalized_value = (value - min_val) / (max_val - min_val)
        store in normalized_array
    
    // Store parameters for later denormalization
    params = create NormalizationParams
    params.min_value = min_val
    params.max_value = max_val
    params.normalization_type = MIN_MAX
    
    return normalized_array, params

minmax_denormalize(normalized_array, size, params):
    create original_array
    
    for each normalized_value:
        original_value = normalized_value * (params.max_value - params.min_value) + params.min_value
        store in original_array
    
    return original_array
```

## Z-Score Standardization
```
zscore_standardize(data_array, size):
    mean = calculate_mean(data_array, size)
    std_dev = calculate_std_dev(data_array, size, mean)
    
    // Handle edge case where std_dev is zero
    if std_dev is approximately zero:
        return array of zeros or handle specially
    
    create standardized_array
    
    for each value in data_array:
        standardized_value = (value - mean) / std_dev
        store in standardized_array
    
    // Store parameters for later inverse operation
    params = create NormalizationParams
    params.mean_value = mean
    params.std_dev_value = std_dev
    params.normalization_type = Z_SCORE
    
    return standardized_array, params

zscore_inverse(standardized_array, size, params):
    create original_array
    
    for each standardized_value:
        original_value = standardized_value * params.std_dev_value + params.mean_value
        store in original_array
    
    return original_array
```

## Multi-Dimensional Data Normalization
```
normalize_stock_data(stock_data_array, num_records):
    // Separate each feature into its own array
    extract open_prices into array
    extract high_prices into array
    extract low_prices into array
    extract close_prices into array
    extract volumes into array
    
    // Normalize each feature independently
    normalized_open, open_params = minmax_normalize(open_prices)
    normalized_high, high_params = minmax_normalize(high_prices)
    normalized_low, low_params = minmax_normalize(low_prices)
    normalized_close, close_params = minmax_normalize(close_prices)
    normalized_volume, volume_params = minmax_normalize(volumes)
    
    // Or use z-score for some features:
    // normalized_volume, volume_params = zscore_standardize(volumes)
    
    // Store all parameters
    normalizer = create DataNormalizer
    normalizer.params[0] = open_params
    normalizer.params[1] = high_params
    normalizer.params[2] = low_params
    normalizer.params[3] = close_params
    normalizer.params[4] = volume_params
    
    // Reconstruct normalized stock records
    create normalized_stock_array
    for each record index:
        create new StockRecord
        record.open = normalized_open[index]
        record.high = normalized_high[index]
        record.low = normalized_low[index]
        record.close = normalized_close[index]
        record.volume = normalized_volume[index]
        add to normalized_stock_array
    
    return normalized_stock_array, normalizer

denormalize_stock_data(normalized_stock_array, normalizer):
    // Extract normalized features
    extract normalized features into separate arrays
    
    // Denormalize each feature
    original_open = minmax_denormalize(normalized_open, open_params)
    original_high = minmax_denormalize(normalized_high, high_params)
    // ... repeat for all features
    
    // Reconstruct original stock records
    create original_stock_array
    for each record index:
        reconstruct StockRecord with original values
    
    return original_stock_array
```

## Numerical Stability Considerations
```
safe_divide(numerator, denominator):
    epsilon = very small value (e.g., 1e-8)
    if absolute(denominator) < epsilon:
        return 0 or some safe default
    return numerator / denominator

safe_sqrt(value):
    if value < 0:
        return 0 or handle error
    return square_root(value)

check_for_outliers(data_array, size):
    mean = calculate_mean(data_array, size)
    std_dev = calculate_std_dev(data_array, size, mean)
    
    for each value:
        z_score = (value - mean) / std_dev
        if absolute(z_score) > threshold (e.g., 3 or 4):
            flag as potential outlier
```

## Testing Strategy
```
test_minmax_normalization():
    // Create test data
    test_data = [10.0, 20.0, 30.0, 40.0, 50.0]
    
    // Normalize
    normalized, params = minmax_normalize(test_data)
    
    // Verify all values in [0, 1]
    assert all values >= 0.0
    assert all values <= 1.0
    assert first value == 0.0
    assert last value == 1.0
    
    // Denormalize
    original = minmax_denormalize(normalized, params)
    
    // Verify we get originals back (within tolerance)
    for each index:
        assert absolute(original[index] - test_data[index]) < epsilon
    
    print "Min-Max test passed"

test_zscore_standardization():
    // Create test data with known mean and std
    test_data = [10.0, 20.0, 30.0, 40.0, 50.0]
    
    // Standardize
    standardized, params = zscore_standardize(test_data)
    
    // Verify mean is approximately 0
    new_mean = calculate_mean(standardized)
    assert absolute(new_mean) < epsilon
    
    // Verify std dev is approximately 1
    new_std = calculate_std_dev(standardized, new_mean)
    assert absolute(new_std - 1.0) < epsilon
    
    // Inverse transform
    original = zscore_inverse(standardized, params)
    
    // Verify we get originals back
    for each index:
        assert absolute(original[index] - test_data[index]) < epsilon
    
    print "Z-Score test passed"

test_multidimensional_normalization():
    // Create sample stock data
    create 5-10 stock records with varied prices
    
    // Normalize
    normalized_data, normalizer = normalize_stock_data(stock_data)
    
    // Verify each feature is normalized
    for each feature:
        extract normalized feature values
        assert all values in expected range [0,1] for min-max
        or assert mean ≈ 0 and std ≈ 1 for z-score
    
    // Denormalize
    original_data = denormalize_stock_data(normalized_data, normalizer)
    
    // Verify reconstruction
    for each record:
        for each field:
            assert original matches within tolerance
    
    print "Multi-dimensional test passed"

test_edge_cases():
    // All same values
    test_data = [5.0, 5.0, 5.0, 5.0]
    verify doesn't crash, returns reasonable result
    
    // Single value
    test_data = [42.0]
    verify handles gracefully
    
    // Very large range
    test_data = [0.001, 1000000.0]
    verify numerical stability
    
    // Negative values
    test_data = [-100.0, -50.0, 0.0, 50.0, 100.0]
    verify works correctly with negatives
    
    print "Edge cases test passed"
```

## Integration with Stock Data
```
prepare_stock_data_for_rnn(csv_filename):
    // Read raw data
    raw_stock_data = read_stock_csv(csv_filename)
    
    // Normalize
    normalized_data, normalizer = normalize_stock_data(raw_stock_data)
    
    // Save normalizer for later use with predictions
    save_normalizer_params(normalizer, "normalizer.dat")
    
    return normalized_data

use_rnn_predictions(predictions, normalizer_file):
    // Load normalizer
    normalizer = load_normalizer_params(normalizer_file)
    
    // Denormalize predictions back to actual prices
    actual_prices = denormalize_predictions(predictions, normalizer)
    
    return actual_prices
```

## File I/O for Parameters
```
save_normalizer(normalizer, filename):
    open file for writing
    write number_of_features
    for each feature:
        write normalization_type
        write min, max, mean, std_dev (as applicable)
    close file

load_normalizer(filename):
    open file for reading
    read number_of_features
    create normalizer
    for each feature:
        read parameters
        populate normalizer
    close file
    return normalizer

*/
