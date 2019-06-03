#include "util.h"

bool try_cast_int (char *num, int *result) {
    int sz = strlen(num);
    
    for (int i = sz - 1, n = 1; i >= 0; i--, n *= 10) {
        if (num[i] >= '0' && num[i] <= '9') {
            (*result) += n * (num[i] - '0');
        } else {
            return false;
        }
    }

    return true;
}