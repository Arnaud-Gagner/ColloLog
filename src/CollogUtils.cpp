#include "CollogUtils.h"

#ifdef _WIN32
#include <intrin.h>
uint64_t rdtsc()
{
    return __rdtsc();
}
#else
uint64_t rdtsc()
{
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return (static_cast<uint64_t>(hi) << 32) | lo;
}
#endif
