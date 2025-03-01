#include <unistd.h>

void *ymalloc(size_t);
void free(void *);

typedef enum {
    FIRST_FIT,
    BEST_FIT
} placement_t;

