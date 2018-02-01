#ifndef MURMURHASH_H
#define MURMURHASH_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

// murmur3 hash function
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) ;

#endif // MURMURHASH_H
