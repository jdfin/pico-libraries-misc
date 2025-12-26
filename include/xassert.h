#pragma once

#ifndef xassert

void xassert_func(const char *file, int line, const char *expr);

#define xassert(__e) ((__e) ? (void)0 : xassert_func(__FILE__, __LINE__, #__e))

#endif