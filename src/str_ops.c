#include <stdlib.h>
#include "str_ops.h"


int str_to_int(const char *str, int *i)
{
    char *str_end;
    int new_i = strtol(str, &str_end, 0);
    if (*str_end != '\0')
        return 0; // i not changed
    *i = new_i;
    return 1;
}