#include <assert.h>
#include "ymalloc.h"

void test_new_allocation_from_os() {
    int *num = ymalloc(sizeof(int), FIRST_FIT);
    assert (num != NULL);
    free(num);
}

void test_reuse_single_block() {
    int *num1 = ymalloc(sizeof(int), FIRST_FIT);
    assert (num1 != NULL);
    free(num1);
    int *num2 = ymalloc(sizeof(int), FIRST_FIT);
    assert (num1 == num2);
    free(num2);
}

void test_reuse_first_fit() {
    int *num1 = ymalloc(sizeof(int), FIRST_FIT);
    assert (num1 != NULL);
    int *num2 = ymalloc(sizeof(int) * 3, FIRST_FIT);
    assert (num2 != NULL);
    int *num3 = ymalloc(sizeof(int) * 2, FIRST_FIT);
    assert (num3 != NULL);
    free(num2);
    free(num3);
    // although num3 is free and is 2 bytes,
    // we should get back num2 because its first fit.
    int *num4 = ymalloc(sizeof(int), FIRST_FIT);
    assert(num4 == num2);
    free(num4);
}

void test_reuse_best_fit() {
    int *num1 = ymalloc(sizeof(int), BEST_FIT);
    assert (num1 != NULL);
    int *num2 = ymalloc(sizeof(int) * 3, BEST_FIT);
    assert (num2 != NULL);
    int *num3 = ymalloc(sizeof(int) * 2, BEST_FIT);
    assert (num3 != NULL);
    free(num2);
    free(num3);
    int *num4 = ymalloc(sizeof(int), BEST_FIT);
    assert (num4 == num3);
    free(num4);

}

int main() {
    test_new_allocation_from_os();
    test_reuse_single_block();
    test_reuse_first_fit();
    test_reuse_best_fit();
}
