#pragma once


#ifdef __cplusplus
extern "C" {
#endif


int str_to_int(const char *str, int *i);

// Like strncpy, except:
// - always null terminates dst (as long as dst_max > 0)
// - does not pad with nulls if src is shorter than dst_max
inline char *strxcpy(char *dst, const char *src, int dst_max)
{
    if (dst_max == 0)
        return dst;
    int i;
    for (i = 0; i < (dst_max - 1) && src[i] != '\0'; i++)
        dst[i] = src[i];
    dst[i] = '\0';
    return dst;
}


#ifdef __cplusplus
}
#endif