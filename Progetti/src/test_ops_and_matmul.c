#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "stack.h"
#include "tensor.h"

void op_and(tf_stack_t *stack);
void op_matmul(tf_stack_t *stack);

static void fail(const char *msg) {
    fprintf(stderr, "TEST FALLITO: %s\n", msg);
    exit(1);
}

static tensor_t *make_vec3_logic(float x0, float x1, float x2) {
    int32_t shape[] = {3};
    tensor_t *t = tensor_alloc(shape, 1);
    t->data[0] = x0;
    t->data[1] = x1;
    t->data[2] = x2;
    return t;
}

static tensor_t *make_matrix(int32_t rows, int32_t cols, const float *data) {
    int32_t shape[] = {rows, cols};
    tensor_t *t = tensor_alloc(shape, 2);
    for (int32_t i = 0; i < rows * cols; i++) {
        t->data[i] = data[i];
    }
    return t;
}

static void print_vec3(const char *label, const tensor_t *t) {
    printf("%s = [%.2f, %.2f, %.2f]\n", label, t->data[0], t->data[1], t->data[2]);
}

static void print_matrix(const char *label, const tensor_t *t) {
    printf("%s (%dx%d) =\n", label, t->shape[0], t->shape[1]);
    for (int32_t i = 0; i < t->shape[0]; i++) {
        printf("  [");
        for (int32_t j = 0; j < t->shape[1]; j++) {
            printf("%.2f%s", t->data[i * t->shape[1] + j], j < t->shape[1] - 1 ? ", " : "");
        }
        printf("]\n");
    }
}

static void print_and_details(const tensor_t *a, const tensor_t *b, const tensor_t *result) {
    printf("\n[AND] dettagli\n");
    print_vec3("A", a);
    print_vec3("B", b);
    for (int i = 0; i < 3; i++) {
        printf("A[%d] & B[%d] = %.0f & %.0f = %.0f\n", i, i, a->data[i], b->data[i], result->data[i]);
    }
    print_vec3("Risultato AND", result);
}

static void assert_vec3_eq(const tensor_t *t, float e0, float e1, float e2) {
    const float eps = 1e-6f;
    if (t->ndim != 1 || t->shape[0] != 3) {
        fail("shape attesa [3]");
    }
    if (fabsf(t->data[0] - e0) > eps || fabsf(t->data[1] - e1) > eps || fabsf(t->data[2] - e2) > eps) {
        fail("valori inattesi");
    }
}

static void assert_matrix_eq(const tensor_t *t, int32_t exp_rows, int32_t exp_cols, const float *expected) {
    const float eps = 1e-6f;
    if (t->ndim != 2 || t->shape[0] != exp_rows || t->shape[1] != exp_cols) {
        fail("shape matrice attesa mismatch");
    }
    for (int32_t i = 0; i < exp_rows * exp_cols; i++) {
        if (fabsf(t->data[i] - expected[i]) > eps) {
            fail("valori matrice inattesi");
        }
    }
}

static void test_and_vectors(void) {
    printf("\n=== TEST AND vettori ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3_logic(1.0f, 0.0f, 1.0f);
    tensor_t *b = make_vec3_logic(1.0f, 1.0f, 0.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_and(&stack);

    if (stack.top != 0) {
        fail("op_and deve lasciare un solo risultato nello stack");
    }

    tensor_t *result = stack_pop_tensor(&stack);
    print_and_details(a, b, result);
    /* Risultato atteso: [1.0 & 1.0 = 1.0, 0.0 & 1.0 = 0.0, 1.0 & 0.0 = 0.0] */
    assert_vec3_eq(result, 1.0f, 0.0f, 0.0f);

    printf("✅ test_and_vectors PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_and_all_ones(void) {
    printf("\n=== TEST AND con tutti 1.0 ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3_logic(1.0f, 1.0f, 1.0f);
    tensor_t *b = make_vec3_logic(1.0f, 1.0f, 1.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_and(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    print_and_details(a, b, result);
    assert_vec3_eq(result, 1.0f, 1.0f, 1.0f);

    printf("✅ test_and_all_ones PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_and_all_zeros(void) {
    printf("\n=== TEST AND con tutti 0.0 ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3_logic(0.0f, 0.0f, 0.0f);
    tensor_t *b = make_vec3_logic(0.0f, 0.0f, 0.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_and(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    print_and_details(a, b, result);
    assert_vec3_eq(result, 0.0f, 0.0f, 0.0f);

    printf("✅ test_and_all_zeros PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_matmul_2x3_times_3x2(void) {
    printf("\n=== TEST MATMUL (2x3) @ (3x2) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    /* A = [1 2 3]
           [4 5 6]  (2x3) */
    float a_data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    tensor_t *a = make_matrix(2, 3, a_data);

    /* B = [7 8]
           [9 10]
           [11 12]  (3x2) */
    float b_data[] = {7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f};
    tensor_t *b = make_matrix(3, 2, b_data);

    printf("\nInput A:\n");
    print_matrix("A", a);
    printf("\nInput B:\n");
    print_matrix("B", b);

    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);

    op_matmul(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    printf("\nRisultato A @ B:\n");
    print_matrix("Risultato", result);

    /* Calcolo atteso:
       [1*7 + 2*9 + 3*11, 1*8 + 2*10 + 3*12] = [58, 64]
       [4*7 + 5*9 + 6*11, 4*8 + 5*10 + 6*12] = [139, 154] */
    float expected[] = {58.0f, 64.0f, 139.0f, 154.0f};
    assert_matrix_eq(result, 2, 2, expected);

    printf("✅ test_matmul_2x3_times_3x2 PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_matmul_3x2_times_2x3(void) {
    printf("\n=== TEST MATMUL (3x2) @ (2x3) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    /* A = [1 2]
           [3 4]
           [5 6]  (3x2) */
    float a_data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    tensor_t *a = make_matrix(3, 2, a_data);

    /* B = [7 8 9]
           [10 11 12]  (2x3) */
    float b_data[] = {7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f};
    tensor_t *b = make_matrix(2, 3, b_data);

    printf("\nInput A:\n");
    print_matrix("A", a);
    printf("\nInput B:\n");
    print_matrix("B", b);

    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);

    op_matmul(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    printf("\nRisultato A @ B:\n");
    print_matrix("Risultato", result);

    /* Calcolo atteso:
       [1*7 + 2*10, 1*8 + 2*11, 1*9 + 2*12] = [27, 30, 33]
       [3*7 + 4*10, 3*8 + 4*11, 3*9 + 4*12] = [61, 68, 75]
       [5*7 + 6*10, 5*8 + 6*11, 5*9 + 6*12] = [95, 106, 117] */
    float expected[] = {27.0f, 30.0f, 33.0f, 61.0f, 68.0f, 75.0f, 95.0f, 106.0f, 117.0f};
    assert_matrix_eq(result, 3, 3, expected);

    printf("✅ test_matmul_3x2_times_2x3 PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_matmul_identity(void) {
    printf("\n=== TEST MATMUL con matrice identità ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    /* A = [2 3]
           [4 5]  (2x2) */
    float a_data[] = {2.0f, 3.0f, 4.0f, 5.0f};
    tensor_t *a = make_matrix(2, 2, a_data);

    /* I = [1 0]
           [0 1]  (2x2) identità */
    float i_data[] = {1.0f, 0.0f, 0.0f, 1.0f};
    tensor_t *i = make_matrix(2, 2, i_data);

    printf("\nInput A:\n");
    print_matrix("A", a);
    printf("\nInput I (identità):\n");
    print_matrix("I", i);

    stack_push_tensor(&stack, i);
    stack_push_tensor(&stack, a);

    op_matmul(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    printf("\nRisultato A @ I:\n");
    print_matrix("Risultato", result);

    /* A @ I = A */
    assert_matrix_eq(result, 2, 2, a_data);

    printf("✅ test_matmul_identity PASSATO\n");

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(i);
}

static double elapsed_ms(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_nsec - start.tv_nsec) / 1000000.0;
}

static void benchmark_matmul(void) {
    printf("\n=== BENCHMARK MATMUL ===\n");

    const int32_t sizes[] = {64, 128, 256, 512};
    const size_t count = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n%-10s  Tempo (ms)\n", "Dimensione");
    printf("-----------------------------\n");

    for (size_t idx = 0; idx < count; idx++) {
        int32_t n = sizes[idx];
        
        float *a_data = (float *)malloc(n * n * sizeof(float));
        float *b_data = (float *)malloc(n * n * sizeof(float));

        for (int32_t i = 0; i < n * n; i++) {
            a_data[i] = (float)(i % 10) + 1.0f;
            b_data[i] = (float)(i % 7) + 1.0f;
        }

        tensor_t *a = make_matrix(n, n, a_data);
        tensor_t *b = make_matrix(n, n, b_data);

        tf_stack_t stack;
        stack_init(&stack);

        stack_push_tensor(&stack, b);
        stack_push_tensor(&stack, a);

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        op_matmul(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double ms = elapsed_ms(start, end);
        printf("%-10dx%-5d %10.3f\n", n, n, ms);

        tensor_t *result = stack_pop_tensor(&stack);
        tensor_decref(result);
        tensor_decref(a);
        tensor_decref(b);
        free(a_data);
        free(b_data);
    }
    printf("-----------------------------\n");
}

int main(void) {
    printf("=== Test operatori AND e MATMUL ===\n");

    /* Test operatore AND */
    test_and_vectors();
    test_and_all_ones();
    test_and_all_zeros();

    /* Test operatore MATMUL */
    test_matmul_2x3_times_3x2();
    test_matmul_3x2_times_2x3();
    test_matmul_identity();

    /* Benchmark */
    benchmark_matmul();

    printf("\n✅ OK: tutti i test operazioni AND e MATMUL completati.\n");
    return 0;
}
