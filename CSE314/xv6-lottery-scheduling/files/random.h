/*
*  Source: https://www.cs.virginia.edu/~cr4bd/4414/S2019/files/lcg_parkmiller_c.txt
*/

#include "types.h"
#define RANDOM_MAX ((1u << 31u) - 1u)

unsigned lcg_parkmiller(unsigned *state)
{
    const unsigned N = 0x7fffffff;
    const unsigned G = 48271u;
    unsigned div = *state / (N / G);  /* max : 2,147,483,646 / 44,488 = 48,271 */
    unsigned rem = *state % (N / G);  /* max : 2,147,483,646 % 44,488 = 44,487 */
    unsigned a = rem * G;        /* max : 44,487 * 48,271 = 2,147,431,977 */
    unsigned b = div * (N % G);  /* max : 48,271 * 3,399 = 164,073,129 */
    return *state = (a > b) ? (a - b) : (a + (N - b));
}

unsigned rand(unsigned int seed) {
    return lcg_parkmiller(&seed);
}
