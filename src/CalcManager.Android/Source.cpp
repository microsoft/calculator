#include <locale.h>

// see https://github.com/aosp-mirror/platform_bionic/commit/3103f6d39e4ae309a9374ee38f414400c889c558#diff-71865bddb52592934317bb2340a94b0cR199

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
