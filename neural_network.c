#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "matrix.h"
#include "float_matrix.h"

extern double sigmoid(double x);
extern double sigmoid_derivative(double x);

typedef struct {
    Matrix *weights;
    Matrix *biases;
} NeuralNetwork;

NeuralNetwork* create_neural_network(int input_size, int output_size) {
    NeuralNetwork *nn = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    if (nn == NULL) {
        return NULL;
    }

    nn->weights = create_matrix(input_size, output_size);
    if (nn->weights == NULL) {
        free(nn);
        return NULL;
    }

    for (int i = 0; i < input_size; i++) {
        for (int j = 0; j < output_size; j++) {
            nn->weights->data[i][j] = (rand() % 201) - 100;
        }
    }
    nn->biases = create_matrix(1, output_size);
    if (nn->biases == NULL) {
        dealloc_matrix(nn->weights);
        free(nn);
        return NULL;
    }

    for (int j = 0; j < output_size; j++) {
        nn->biases->data[0][j] = 0;
    }

    return nn;
}

void dealloc_neural_network(NeuralNetwork *nn) {
    if (nn == NULL) return;
    dealloc_matrix(nn->weights);
    dealloc_matrix(nn->biases);
    free(nn);
}

Matrix* forward_pass(NeuralNetwork *nn, Matrix *input) {
    Matrix *weighted_sum = multiply_matrix(input, nn->weights);
    if (weighted_sum == NULL) {
        return NULL;
    }

    Matrix *output = create_matrix(1, weighted_sum->cols);
    if (output == NULL) {
        dealloc_matrix(weighted_sum);
        return NULL;
    }

    for (int j = 0; j < weighted_sum->cols; j++) {
        double z = (weighted_sum->data[0][j] + nn->biases->data[0][j]) / 100.0;
        output->data[0][j] = (int)(sigmoid(z) * 100);
    }

    dealloc_matrix(weighted_sum);
    return output;
}

void train_step(NeuralNetwork *nn, Matrix *input, Matrix *target, double learning_rate) {
    Matrix *output = forward_pass(nn, input);
    if (output == NULL) {
        return;
    }

    Matrix *gradient = create_matrix(1, output->cols);
    if (gradient == NULL) {
        dealloc_matrix(output);
        return;
    }

    for (int j = 0; j < output->cols; j++) {
        double error = target->data[0][j] - output->data[0][j];
        double output_val = output->data[0][j] / 100.0;
        gradient->data[0][j] = (int)(error * sigmoid_derivative(output_val) * 100);
    }

    Matrix *input_T = transpose(input);
    if (input_T != NULL) {
        Matrix *weight_delta = multiply_matrix(input_T, gradient);
        if (weight_delta != NULL) {
            for (int i = 0; i < nn->weights->rows; i++) {
                for (int j = 0; j < nn->weights->cols; j++) {
                    nn->weights->data[i][j] += (int)(learning_rate * weight_delta->data[i][j] / 100.0);
                }
            }
            dealloc_matrix(weight_delta);
        }
        dealloc_matrix(input_T);
    }

    for (int j = 0; j < nn->biases->cols; j++) {
        nn->biases->data[0][j] += (int)(learning_rate * gradient->data[0][j] / 100.0);
    }

    dealloc_matrix(output);
    dealloc_matrix(gradient);
}

void test_xor() {
    printf("\n=== Testing Neural Network on XOR ===\n");
    printf("Note: This is a simplified example with integer matrices\n");
    printf("For production neural networks, use FloatMatrix instead\n\n");

    NeuralNetwork *nn = create_neural_network(2, 2);
    if (nn == NULL) {
        printf("Failed to create neural network\n");
        return;
    }

    int inputs[4][2] = {
        {0, 0},
        {0, 100},
        {100, 0},
        {100, 100}
    };

    int targets[4][2] = {
        {0, 0},
        {100, 0},
        {100, 0},
        {0, 100}
    };

    printf("Training for 1000 epochs...\n");
    for (int epoch = 0; epoch < 1000; epoch++) {
        for (int sample = 0; sample < 4; sample++) {
            Matrix *input = create_matrix(1, 2);
            input->data[0][0] = inputs[sample][0];
            input->data[0][1] = inputs[sample][1];

            Matrix *target = create_matrix(1, 2);
            target->data[0][0] = targets[sample][0];
            target->data[0][1] = targets[sample][1];

            train_step(nn, input, target, 0.5);

            dealloc_matrix(input);
            dealloc_matrix(target);
        }

        if (epoch % 200 == 0) {
            printf("  Epoch %d complete\n", epoch);
        }
    }

    printf("\nTesting trained network:\n");
    printf("Input -> Output (Expected)\n");
    for (int sample = 0; sample < 4; sample++) {
        Matrix *input = create_matrix(1, 2);
        input->data[0][0] = inputs[sample][0];
        input->data[0][1] = inputs[sample][1];

        Matrix *output = forward_pass(nn, input);
        if (output != NULL) {
            printf("[%d, %d] -> %d (Expected: %d)\n",
                   inputs[sample][0] / 100,
                   inputs[sample][1] / 100,
                   output->data[0][0] / 100,
                   targets[sample][0] / 100);
            dealloc_matrix(output);
        }

        dealloc_matrix(input);
    }

    dealloc_neural_network(nn);
}

int main() {
    srand(time(NULL));

    printf("Neural Network Demo\n");
    printf("===================\n");

    test_xor();

    printf("\n✓ Neural network test completed!\n");
    return 0;
}