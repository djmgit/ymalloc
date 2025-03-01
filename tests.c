#include <assert.h>
#include "ymalloc.h"

void test_new_allocation_from_os() {
    int *num = ymalloc(sizeof(int));
    assert (num != NULL);
    free(num);
}

void test_reuse_single_block() {
    int *num1 = ymalloc(sizeof(int));
    assert (num1 != NULL);
    free(num1);
    int *num2 = ymalloc(sizeof(int));
    assert (num1 == num2);
    free(num2);
}

void test_reuse_first_fit() {
    int *num1 = ymalloc(sizeof(int));
    assert (num1 != NULL);
    int *num2 = ymalloc(sizeof(int) * 3);
    assert (num2 != NULL);
    int *num3 = ymalloc(sizeof(int) * 2);
    assert (num3 != NULL);
    free(num2);
    free(num3);
    // although num3 is free and is 2 bytes,
    // we should get back num2 because its first fit.
    int *num4 = ymalloc(sizeof(int));
    assert(num4 == num2);
    free(num4);
}

int main() {
    test_new_allocation_from_os();
    test_reuse_single_block();
    test_reuse_first_fit();
}
