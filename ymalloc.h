#include <unistd.h>

typedef enum {
    FIRST_FIT,
    BEST_FIT
} placement_t;

void *ymalloc(size_t, placement_t);
void free(void *);
