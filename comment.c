/* 
 
 FUNCTION relu(input_value):
    IF input_value > 0.0 THEN
        RETURN input_value
    ELSE
        RETURN 0.0
    END IF
END FUNCTION

FUNCTION relu_derivative(input_value):
    IF input_value > 0.0 THEN
        RETURN 1.0
    ELSE
        RETURN 0.0
    END IF
END FUNCTION

FUNCTION leaky_relu(input_value, alpha):
    IF input_value > 0.0 THEN
        RETURN input_value
    ELSE
        RETURN alpha * input_value
    END IF
END FUNCTION

FUNCTION leaky_relu_derivative(input_value, alpha):
    IF input_value > 0.0 THEN
        RETURN 1.0
    ELSE
        RETURN alpha
    END IF
END FUNCTION
```

## **Softmax (array version - needs array input)**
```
FUNCTION softmax(input_array, output_array, array_length):
    // Find max for numerical stability
    max_val = input_array[0]
    FOR i FROM 1 TO array_length - 1:
        IF input_array[i] > max_val THEN
            max_val = input_array[i]
        END IF
    END FOR
    
    // Calculate sum of exponentials
    sum_exp = 0.0
    FOR i FROM 0 TO array_length - 1:
        sum_exp = sum_exp + exp(input_array[i] - max_val)
    END FOR
    
    // Calculate softmax values
    FOR i FROM 0 TO array_length - 1:
        output_array[i] = exp(input_array[i] - max_val) / sum_exp
    END FOR
END FUNCTION
```

## **Hard Sigmoid (scalar)**
```
FUNCTION hard_sigmoid(input_value):
    result = 0.2 * input_value + 0.5
    
    IF result < 0.0 THEN
        RETURN 0.0
    ELSE IF result > 1.0 THEN
        RETURN 1.0
    ELSE
        RETURN result
    END IF
END FUNCTION

FUNCTION hard_sigmoid_derivative(input_value):
    IF input_value < -2.5 OR input_value > 2.5 THEN
        RETURN 0.0
    ELSE
        RETURN 0.2
    END IF
END FUNCTION
```

## **Linear/Identity (scalar)**
```
FUNCTION linear(input_value):
    RETURN input_value
END FUNCTION

FUNCTION linear_derivative(input_value):
    RETURN 1.0
END FUNCTION
```

## **ELU - Exponential Linear Unit (scalar)**
```
FUNCTION elu(input_value, alpha):
    // alpha typically 1.0
    IF input_value > 0.0 THEN
        RETURN input_value
    ELSE
        RETURN alpha * (exp(input_value) - 1.0)
    END IF
END FUNCTION

FUNCTION elu_derivative(input_value, alpha):
    IF input_value > 0.0 THEN
        RETURN 1.0
    ELSE
        RETURN elu(input_value, alpha) + alpha
    END IF
END FUNCTION
```

## **Swish (scalar)**
```
FUNCTION swish(input_value):
    RETURN input_value * sigmoid(input_value)
END FUNCTION

FUNCTION swish_derivative(input_value):
    sig_val = sigmoid(input_value)
    RETURN sig_val + input_value * sig_val * (1.0 - sig_val)
END FUNCTION
 
 */
