#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "stack.h"
#include "tensor.h"

void op_add(tf_stack_t *stack);
void op_sub(tf_stack_t *stack);
void op_mul(tf_stack_t *stack);
void op_lt(tf_stack_t *stack);
void op_gt(tf_stack_t *stack);
void op_eq(tf_stack_t *stack);

static void fail(const char *msg) {
    fprintf(stderr, "TEST FALLITO: %s\n", msg);
    exit(1);
}

static tensor_t *make_vec3(float x0, float x1, float x2) {
    int32_t shape[] = {3};
    tensor_t *t = tensor_alloc(shape, 1);
    t->data[0] = x0;
    t->data[1] = x1;
    t->data[2] = x2;
    return t;
}

static void print_vec3(const char *label, const tensor_t *t) {
    printf("%s = [%.2f, %.2f, %.2f]\n", label, t->data[0], t->data[1], t->data[2]);
}

static void print_add_details(const tensor_t *a, const tensor_t *b, const tensor_t *result) {
    printf("\n[ADD] dettagli\n");
    print_vec3("A", a);
    print_vec3("B", b);
    for (int i = 0; i < 3; i++) {
        printf("A[%d] + B[%d] = %.2f + %.2f = %.2f\n", i, i, a->data[i], b->data[i], result->data[i]);
    }
    print_vec3("Risultato somma", result);
}

static void print_sub_details(const tensor_t *a, const tensor_t *b, const tensor_t *result) {
    printf("\n[SUB] dettagli\n");
    print_vec3("A", a);
    print_vec3("B", b);
    for (int i = 0; i < 3; i++) {
        printf("Top[%d] - Sotto[%d] = %.2f - %.2f = %.2f\n", i, i, b->data[i], a->data[i], result->data[i]);
    }
    print_vec3("Risultato sottrazione", result);
}

static tensor_t *make_vector(int32_t size, float base, float step) {
    int32_t shape[] = { size };
    tensor_t *t = tensor_alloc(shape, 1);
    for (int32_t i = 0; i < size; i++) {
        t->data[i] = base + step * (float)i;
    }
    return t;
}

static double elapsed_ms(struct timespec start, struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_nsec - start.tv_nsec) / 1000000.0;
}

static void print_benchmark_header(int32_t size) {
    printf("\n============================================================\n");
    printf("Benchmark vettori: n=%d\n", size);
    printf("------------------------------------------------------------\n");
    printf("operazione   tempo (ms)   campione risultato\n");
    printf("------------------------------------------------------------\n");
}

static void print_benchmark_row(const char *name, double ms, const tensor_t *t) {
    int32_t size = t->shape[0];
    int32_t mid = size / 2;
    printf("%-11s %10.3f   [%.2f, %.2f, %.2f]\n",
           name, ms, t->data[0], t->data[mid], t->data[size - 1]);
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

static void test_add_vectors(void) {
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3(1.0f, 2.0f, 3.0f);
    tensor_t *b = make_vec3(4.0f, 5.0f, 6.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_add(&stack);

    if (stack.top != 0) {
        fail("op_add deve lasciare un solo risultato nello stack");
    }

    tensor_t *result = stack_pop_tensor(&stack);
    print_add_details(a, b, result);
    assert_vec3_eq(result, 5.0f, 7.0f, 9.0f);

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_sub_vectors(void) {
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3(10.0f, 8.0f, 6.0f);
    tensor_t *b = make_vec3(1.0f, 2.0f, 3.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_sub(&stack);

    if (stack.top != 0) {
        fail("op_sub deve lasciare un solo risultato nello stack");
    }

    tensor_t *result = stack_pop_tensor(&stack);
    print_sub_details(a, b, result);
    /* In op_sub l'implementazione corrente usa: top - sotto-top. */
    assert_vec3_eq(result, -9.0f, -6.0f, -3.0f);

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_mul_vectors(void) {
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec3(1.0f, 2.0f, 3.0f);
    tensor_t *b = make_vec3(4.0f, 5.0f, 6.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    op_mul(&stack);

    if (stack.top != 0) {
        fail("op_mul deve lasciare un solo risultato nello stack");
    }

    tensor_t *result = stack_pop_tensor(&stack);
    print_add_details(a, b, result);
    assert_vec3_eq(result, 4.0f, 10.0f, 18.0f);

    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_compare_vectors(void) {
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *left  = make_vec3(1.0f, 3.0f, 5.0f);
    tensor_t *right = make_vec3(2.0f, 3.0f, 4.0f);

    /* Per l'API: a = top, b = sotto; quindi push prima right poi left */
    stack_push_tensor(&stack, right);
    stack_push_tensor(&stack, left);
    op_lt(&stack);
    tensor_t *lt_res = stack_pop_tensor(&stack);
    assert_vec3_eq(lt_res, 1.0f, 0.0f, 0.0f);

    tensor_decref(lt_res);

    stack_push_tensor(&stack, right);
    stack_push_tensor(&stack, left);
    op_gt(&stack);
    tensor_t *gt_res = stack_pop_tensor(&stack);
    assert_vec3_eq(gt_res, 0.0f, 0.0f, 1.0f);
    tensor_decref(gt_res);

    stack_push_tensor(&stack, right);
    stack_push_tensor(&stack, left);
    op_eq(&stack);
    tensor_t *eq_res = stack_pop_tensor(&stack);
    assert_vec3_eq(eq_res, 0.0f, 1.0f, 0.0f);
    tensor_decref(eq_res);

    tensor_decref(left);
    tensor_decref(right);
}

static void benchmark_add_sub(void) {
    const int32_t sizes[] = { 1000000, 5000000 };
    const size_t count = sizeof(sizes) / sizeof(sizes[0]);

    printf("\n=== BENCHMARK operazioni su vettori da milioni di elementi ===\n");
    for (size_t idx = 0; idx < count; idx++) {
        int32_t size = sizes[idx];
        tf_stack_t stack;
        stack_init(&stack);

        tensor_t *a = make_vector(size, 1.0f, 1.0f);
        tensor_t *b = make_vector(size, 2.0f, 2.0f);

        stack_push_tensor(&stack, a);
        stack_push_tensor(&stack, b);

        struct timespec start_add, end_add;
        clock_gettime(CLOCK_MONOTONIC, &start_add);
        op_add(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_add);

        tensor_t *add_result = stack_pop_tensor(&stack);

        /* mul */
        stack_push_tensor(&stack, a);
        stack_push_tensor(&stack, b);
        struct timespec start_mul, end_mul;
        clock_gettime(CLOCK_MONOTONIC, &start_mul);
        op_mul(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_mul);
        tensor_t *mul_result = stack_pop_tensor(&stack);

        /* lt */
        stack_push_tensor(&stack, b); /* push right */
        stack_push_tensor(&stack, a); /* push left as top to compute left<right */
        struct timespec start_lt, end_lt;
        clock_gettime(CLOCK_MONOTONIC, &start_lt);
        op_lt(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_lt);
        tensor_t *lt_result = stack_pop_tensor(&stack);

        /* gt */
        stack_push_tensor(&stack, b);
        stack_push_tensor(&stack, a);
        struct timespec start_gt, end_gt;
        clock_gettime(CLOCK_MONOTONIC, &start_gt);
        op_gt(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_gt);
        tensor_t *gt_result = stack_pop_tensor(&stack);

        /* eq */
        stack_push_tensor(&stack, b);
        stack_push_tensor(&stack, a);
        struct timespec start_eq, end_eq;
        clock_gettime(CLOCK_MONOTONIC, &start_eq);
        op_eq(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_eq);
        tensor_t *eq_result = stack_pop_tensor(&stack);

        stack_push_tensor(&stack, a);
        stack_push_tensor(&stack, b);

        struct timespec start_sub, end_sub;
        clock_gettime(CLOCK_MONOTONIC, &start_sub);
        op_sub(&stack);
        clock_gettime(CLOCK_MONOTONIC, &end_sub);

        tensor_t *sub_result = stack_pop_tensor(&stack);

        print_benchmark_header(size);
        print_benchmark_row("add", elapsed_ms(start_add, end_add), add_result);
        print_benchmark_row("mul", elapsed_ms(start_mul, end_mul), mul_result);
        print_benchmark_row("lt",  elapsed_ms(start_lt, end_lt),  lt_result);
        print_benchmark_row("gt",  elapsed_ms(start_gt, end_gt),  gt_result);
        print_benchmark_row("eq",  elapsed_ms(start_eq, end_eq),  eq_result);
        print_benchmark_row("sub", elapsed_ms(start_sub, end_sub), sub_result);
        printf("------------------------------------------------------------\n");

        tensor_decref(add_result);
        tensor_decref(mul_result);
        tensor_decref(lt_result);
        tensor_decref(gt_result);
        tensor_decref(eq_result);
        tensor_decref(sub_result);
        tensor_decref(a);
        tensor_decref(b);
    }
}

int main(void) {
    test_add_vectors();
    test_sub_vectors();
    test_mul_vectors();
    test_compare_vectors();
    benchmark_add_sub();
    printf("OK: test operazioni vettoriali completato.\n");
    return 0;
}
