#include <locale.h>

double strtod_l(const char* s, char** end_ptr, locale_t)
{
    return strtod(s, end_ptr);
}

float strtof_l(const char* s, char** end_ptr, locale_t)
{
    return strtof(s, end_ptr);
}

long strtol_l(const char* s, char** end_ptr, int base, locale_t)
{
    return strtol(s, end_ptr, base);
}

unsigned long strtoul_l(const char* s, char** end_ptr, int base, locale_t)
{
    return strtoul(s, end_ptr, base);
}
