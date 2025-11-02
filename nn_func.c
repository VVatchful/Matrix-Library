#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

double sigmoid(double input_value) {
    double negative_input;
    double exp_of_negative;
    double denominator;
    double sigmoid_result;
    negative_input = input_value * -1.0;
    exp_of_negative = exp(negative_input);
    denominator = 1.0 + exp_of_negative;
    sigmoid_result = 1.0 / denominator;
    return sigmoid_result;
}

double sigmoid_error_handl(double input_value) {
    double negative_input;
    double exp_of_negative;
    double denominator;
    double sigmoid_result;

    if (isnan(input_value)) {
        printf("%%1f Error: Input is NaN\n");
        return NAN;
    }
    if (isinf(input_value)) {
        if (input_value > 0.0) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
    if (input_value > 20.0) {
        return 1.0;
    }
    if (input_value < -20.0) {
        return 0.0;
    }

    negative_input = input_value * -1.0;
    exp_of_negative = exp(negative_input);
    denominator = 1.0 + exp_of_negative;
    sigmoid_result = 1.0 / denominator;

    return sigmoid_result;
}

double sigmoid_derivative(double input_value) {
    double sig_val = sigmoid(input_value);
    return sig_val * (1.0 - sig_val);
}   

double tanh_activation(double input_value) {
    double tanh_result;
    tanh_result = tanh(input_value);
    return tanh_result;

}

double tanh_derivative(double input_value) {
    double tanh_val;
    double tanh_squared;
    double derivative_result;

    tanh_val = tanh_activation(input_value);

    tanh_squared = tanh_val * tanh_val;
    derivative_result = 1.0 - tanh_squared;
    
    return derivative_result;
}

double relu(double input_value) {
    if (input_value > 0.0) {
        return input_value;
    } else {
        return 0.0;
    }
}

double relu_derivative(double input_value) {
    if (input_value > 0.0) {
        return 1.0;
    } else {
        return 0.0;
    }
}

double leaky_relu(double input_value) {
    double alpha;
    alpha = 0.01;
    if (input_value > 0.0) {
        return input_value;
    } else {
        return alpha * input_value;
    }
}

double leay_derivative(double input_value, double alpha) {
    if (input_value > 0.0) {
        return 1.0;
    } else {
        return alpha;
    }
}
/*

void softmax(double input_array, double output_array, double array_length) {
    double max_val = input_array[0];
    for (i = 1; i < array_length; i++) {
        if (input_array[i] > max_val) {
            max_val = input_array[i]
        }
    }
}

*/

double hard_sigmoid(double input_value) {
    double result  = 0.2 * input_value + 0.5;
    if (result < 0.0) {
        return 0.0;
    } else if (result > 1.0) {
        return 1.0;
    } else {
        return result;
    }
}

double hard_sigmoid_derivative(double input_value) {
    if (input_value < -2.5 || input_value > 2.5) {
        return 0.0;
    } else {
        return 0.2;
    }
}

double linear(double input_value) {
    return input_value;
}

double linear_derivative(double input_value) {
    return 1.0;
}

double elu(double input_value, double alpha) {
    if (input_value > 0.0) {
        return input_value;
    } else {
        return alpha * (exp(input_value) - 1.0);
    }
}

double elu_derivative(double input_value, double alpha) {
    if (input_value > 0.0) {
        return 1.0;
    } else {
        return elu(input_value, alpha) + alpha;
    }
}

double swish(double input_value) {
    return input_value * sigmoid(input_value);
}
double swish_derivative(double input_value) {
    double sig_val = sigmoid(input_value);
    return sig_val + input_value * sig_val * (1.0 - sig_val);
}

