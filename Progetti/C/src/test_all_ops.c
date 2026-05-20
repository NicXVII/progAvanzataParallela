#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "stack.h"
#include "tensor.h"

/* Operatori aritmetici */
void op_add(tf_stack_t *stack);
void op_sub(tf_stack_t *stack);
void op_mul(tf_stack_t *stack);

/* Operatori di confronto */
void op_lt(tf_stack_t *stack);
void op_gt(tf_stack_t *stack);
void op_eq(tf_stack_t *stack);

/* Operatori logici */
void op_and(tf_stack_t *stack);
void op_or(tf_stack_t *stack);
void op_not(tf_stack_t *stack);
void op_select(tf_stack_t *stack);

/* Operatori per tensori */
void op_matmul(tf_stack_t *stack);
void op_dot(tf_stack_t *stack);
void op_conv(tf_stack_t *stack);

/* Operazioni sulla forma */
void op_reshape(tf_stack_t *stack);
void op_ravel(tf_stack_t *stack);
void op_shape(tf_stack_t *stack);

/* Operazioni casuali */
void op_rand(tf_stack_t *stack);

/* Operazioni elemento per elemento */
void op_relu(tf_stack_t *stack);
void op_min(tf_stack_t *stack);
void op_max(tf_stack_t *stack);

/* Operazioni di riduzione */
void op_sum(tf_stack_t *stack);

/* Operazioni di filling */
void op_fill(tf_stack_t *stack);

/* Operazioni di utilità stack */
void op_dup(tf_stack_t *stack);
void op_swap(tf_stack_t *stack);
void op_over(tf_stack_t *stack);
void op_drop(tf_stack_t *stack);

/* I/O operations */
void op_print(tf_stack_t *stack);

/* ===== Helper functions ===== */

static void fail(const char *msg) {
    fprintf(stderr, "TEST FALLITO: %s\n", msg);
    exit(1);
}

static tensor_t *make_vec(int32_t size, float base, float step) {
    int32_t shape[] = {size};
    tensor_t *t = tensor_alloc(shape, 1);
    for (int32_t i = 0; i < size; i++) {
        t->data[i] = base + step * (float)i;
    }
    return t;
}

static tensor_t *make_logic_vec(const float *data, int32_t size) {
    int32_t shape[] = {size};
    tensor_t *t = tensor_alloc(shape, 1);
    for (int32_t i = 0; i < size; i++) {
        t->data[i] = data[i];
    }
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

static void print_tensor(const char *label, const tensor_t *t) {
    printf("%s (", label);
    for (int i = 0; i < t->ndim; i++) {
        printf("%d%s", t->shape[i], i < t->ndim - 1 ? "x" : "");
    }
    printf(") = [");
    int n = tensor_numel(t);
    for (int i = 0; i < n && i < 10; i++) {
        printf("%.2f%s", t->data[i], i < n - 1 && i < 9 ? ", " : "");
    }
    if (n > 10) printf(", ...");
    printf("]\n");
}

static void assert_float_eq(float a, float b, float eps, const char *msg) {
    if (fabsf(a - b) > eps) {
        fprintf(stderr, "Assertion failed: %.6f != %.6f (%s)\n", a, b, msg);
        fail(msg);
    }
}

static void assert_tensor_eq(const tensor_t *t, int32_t exp_size, const float *expected, 
                             float eps, const char *msg) {
    int n = tensor_numel(t);
    if (n != exp_size) {
        fail("Tensor size mismatch");
    }
    for (int i = 0; i < n; i++) {
        if (fabsf(t->data[i] - expected[i]) > eps) {
            fail(msg);
        }
    }
}

/* ===== Test Aritmetica ===== */

static void test_add(void) {
    printf("\n=== TEST ADD ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]
    tensor_t *b = make_vec(3, 4.0f, 1.0f);  // [4, 5, 6]

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);
    op_add(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {5.0f, 7.0f, 9.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "add failed");

    printf("✅ test_add PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_sub(void) {
    printf("\n=== TEST SUB ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 1.0f);    // [1, 2, 3]
    tensor_t *b = make_vec(3, 10.0f, -1.0f);  // [10, 9, 8]

    /* Op_sub: a (top) - b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_sub(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* [1-10, 2-9, 3-8] = [-9, -7, -5] */
    float expected[] = {-9.0f, -7.0f, -5.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "sub failed");

    printf("✅ test_sub PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_mul(void) {
    printf("\n=== TEST MUL ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]
    tensor_t *b = make_vec(3, 2.0f, 0.0f);  // [2, 2, 2]

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);
    op_mul(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {2.0f, 4.0f, 6.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "mul failed");

    printf("✅ test_mul PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

/* ===== Test Comparazione ===== */

static void test_lt(void) {
    printf("\n=== TEST LT (less than) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 2.0f, 0.0f);  // [2, 2, 2]
    tensor_t *b = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]

    /* Op_lt: a (top) < b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_lt(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* [2<1, 2<2, 2<3] = [0, 0, 1] */
    float expected[] = {0.0f, 0.0f, 1.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "lt failed");

    printf("✅ test_lt PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_gt(void) {
    printf("\n=== TEST GT (greater than) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 2.0f, 0.0f);  // [2, 2, 2]
    tensor_t *b = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]

    /* Op_gt: a (top) > b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_gt(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* [2>1, 2>2, 2>3] = [1, 0, 0] */
    float expected[] = {1.0f, 0.0f, 0.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "gt failed");

    printf("✅ test_gt PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_eq(void) {
    printf("\n=== TEST EQ (equal) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 2.0f, 0.0f);  // [2, 2, 2]
    tensor_t *b = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]

    /* Op_eq: a (top) == b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_eq(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* [2==1, 2==2, 2==3] = [0, 1, 0] */
    float expected[] = {0.0f, 1.0f, 0.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "eq failed");

    printf("✅ test_eq PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

/* ===== Test Logica ===== */

static void test_and(void) {
    printf("\n=== TEST AND ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    float a_data[] = {1.0f, 0.0f, 0.0f};
    float b_data[] = {1.0f, 1.0f, 1.0f};
    tensor_t *a = make_logic_vec(a_data, 3);
    tensor_t *b = make_logic_vec(b_data, 3);

    /* Op_and: a (top) & b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_and(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {1.0f, 0.0f, 0.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "and failed");

    printf("✅ test_and PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_or(void) {
    printf("\n=== TEST OR ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    float a_data[] = {1.0f, 0.0f, 0.0f};
    float b_data[] = {0.0f, 1.0f, 1.0f};
    tensor_t *a = make_logic_vec(a_data, 3);
    tensor_t *b = make_logic_vec(b_data, 3);

    /* Op_or: a (top) | b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_or(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {1.0f, 1.0f, 1.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "or failed");

    printf("✅ test_or PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_not(void) {
    printf("\n=== TEST NOT ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    float a_data[] = {1.0f, 0.0f, 0.0f};
    tensor_t *a = make_logic_vec(a_data, 3);

    stack_push_tensor(&stack, a);
    op_not(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {0.0f, 1.0f, 1.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "not failed");

    printf("✅ test_not PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
}

static void test_select(void) {
    printf("\n=== TEST SELECT ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 1.0f);   // [1, 2, 3] (true values)
    tensor_t *b = make_vec(3, 10.0f, 0.0f);  // [10, 10, 10] (false values)
    
    float m_data[] = {1.0f, 0.0f, 0.0f};
    tensor_t *m = make_logic_vec(m_data, 3);   // [1, 0, 0] (mask)

    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, m);
    op_select(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {1.0f, 10.0f, 10.0f};
    assert_tensor_eq(result, 3, expected, 1e-5, "select failed");

    printf("✅ test_select PASSATO\n");
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
    tensor_decref(m);
}

/* ===== Test Tensori ===== */

static void test_dot(void) {
    printf("\n=== TEST DOT (prodotto scalare) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 1.0f);  // [1, 2, 3]
    tensor_t *b = make_vec(3, 2.0f, 0.0f);  // [2, 2, 2]

    /* Op_dot: a (top) . b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_dot(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* 1*2 + 2*2 + 3*2 = 2 + 4 + 6 = 12 */
    float expected[] = {12.0f};
    assert_tensor_eq(result, 1, expected, 1e-5, "dot failed");

    printf("✅ test_dot PASSATO\n");
    print_tensor("Risultato dot", result);
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_matmul(void) {
    printf("\n=== TEST MATMUL ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    /* A = [1 2]
           [3 4]  (2x2) */
    float a_data[] = {1.0f, 2.0f, 3.0f, 4.0f};
    tensor_t *a = make_matrix(2, 2, a_data);

    /* B = [5 6]
           [7 8]  (2x2) */
    float b_data[] = {5.0f, 6.0f, 7.0f, 8.0f};
    tensor_t *b = make_matrix(2, 2, b_data);

    /* Op_matmul: a (top) @ b (below) */
    stack_push_tensor(&stack, b);
    stack_push_tensor(&stack, a);
    op_matmul(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    /* [1*5 + 2*7, 1*6 + 2*8]   = [19, 22]
       [3*5 + 4*7, 3*6 + 4*8]   = [43, 50] */
    float expected[] = {19.0f, 22.0f, 43.0f, 50.0f};
    assert_tensor_eq(result, 4, expected, 1e-5, "matmul failed");

    printf("✅ test_matmul PASSATO\n");
    print_tensor("Risultato matmul", result);
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(b);
}

static void test_conv(void) {
    printf("\n=== TEST CONV (convoluzione) ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    /* Input 3x3 */
    float a_data[] = {1.0f, 2.0f, 3.0f,
                      4.0f, 5.0f, 6.0f,
                      7.0f, 8.0f, 9.0f};
    tensor_t *a = make_matrix(3, 3, a_data);

    /* Kernel 3x3 (identità per test semplice) */
    float kernel_data[] = {1.0f, 0.0f, 0.0f,
                           0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 1.0f};
    tensor_t *kernel = make_matrix(3, 3, kernel_data);

    printf("Input:\n");
    print_tensor("A", a);
    printf("Kernel:\n");
    print_tensor("K", kernel);

    stack_push_tensor(&stack, kernel);
    stack_push_tensor(&stack, a);
    op_conv(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    printf("Risultato convoluzione:\n");
    print_tensor("Result", result);

    printf("✅ test_conv PASSATO (result shape: %dx%d)\n", result->shape[0], result->shape[1]);
    tensor_decref(result);
    tensor_decref(a);
    tensor_decref(kernel);
}

/* ===== Test Forma ===== */

static void test_shape(void) {
    printf("\n=== TEST SHAPE ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_matrix(3, 4, NULL);  // matrice 3x4
    
    stack_push_tensor(&stack, a);
    op_shape(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    float expected[] = {3.0f, 4.0f};
    assert_tensor_eq(result, 2, expected, 1e-5, "shape failed");

    printf("✅ test_shape PASSATO\n");
    print_tensor("Shape result", result);
    tensor_decref(result);
    tensor_decref(a);
}

static void test_reshape(void) {
    printf("\n=== TEST RESHAPE ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(6, 1.0f, 1.0f);  // [1, 2, 3, 4, 5, 6]
    
    int32_t shape_arr[] = {2};
    tensor_t *shape_tensor = tensor_alloc(shape_arr, 1);
    shape_tensor->data[0] = 2.0f;

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, shape_tensor);
    op_reshape(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    if (result->ndim != 1 || result->shape[0] != 2) {
        fail("reshape shape failed");
    }

    printf("✅ test_reshape PASSATO (new shape: %d)\n", result->shape[0]);
    tensor_decref(result);
    tensor_decref(shape_tensor);
}

static void test_ravel(void) {
    printf("\n=== TEST RAVEL ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    float mat_data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    tensor_t *a = make_matrix(2, 3, mat_data);

    stack_push_tensor(&stack, a);
    op_ravel(&stack);

    tensor_t *result = stack_pop_tensor(&stack);
    if (result->ndim != 1 || result->shape[0] != 6) {
        fail("ravel failed");
    }

    printf("✅ test_ravel PASSATO (new shape: %d)\n", result->shape[0]);
    tensor_decref(result);
}

/* ===== Test Stack Operations ===== */

static void test_drop(void) {
    printf("\n=== TEST DROP ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(3, 1.0f, 0.0f);
    tensor_t *b = make_vec(3, 2.0f, 0.0f);

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    if (stack.top != 1) {
        fail("Stack size before drop incorrect");
    }

    op_drop(&stack);

    if (stack.top != 0) {
        fail("Stack size after drop incorrect");
    }

    printf("✅ test_drop PASSATO\n");
    tensor_decref(a);
}

static void test_over(void) {
    printf("\n=== TEST OVER ===\n");
    tf_stack_t stack;
    stack_init(&stack);

    tensor_t *a = make_vec(2, 1.0f, 0.0f);  // [1, 1]
    tensor_t *b = make_vec(2, 2.0f, 0.0f);  // [2, 2]

    stack_push_tensor(&stack, a);
    stack_push_tensor(&stack, b);

    if (stack.top != 1) {
        fail("Stack size before over incorrect");
    }

    op_over(&stack);

    if (stack.top != 2) {
        fail("Stack size after over incorrect");
    }

    printf("✅ test_over PASSATO\n");
    tensor_decref(a);
    tensor_decref(b);
}

/* ===== Main ===== */

int main(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        Test Suite Completo per Tutte le Operazioni          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    printf("\n--- Operazioni Aritmetiche ---\n");
    test_add();
    test_sub();
    test_mul();

    printf("\n--- Operazioni di Comparazione ---\n");
    test_lt();
    test_gt();
    test_eq();

    printf("\n--- Operazioni Logiche ---\n");
    test_and();
    test_or();
    test_not();
    test_select();

    printf("\n--- Operazioni su Tensori ---\n");
    test_dot();
    test_matmul();
    test_conv();

    printf("\n--- Operazioni sulla Forma ---\n");
    test_shape();
    test_reshape();
    test_ravel();

    printf("\n--- Operazioni Stack ---\n");
    test_drop();
    test_over();

    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ ALL TESTS PASSED - Test suite completo superato!        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}
