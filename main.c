#include <stdlib.h>
#include <limits.h>
#include <stdarg.h> // va_list(類型), (巨集) va_start, va_arg, va_end

typedef unsigned int (*hashfunc_t)(const char *);
typedef struct  // C 語言中 typedef 可以用來擴充 C 原有的資料型態. 通常我們會將某個資料型態或者將常用的資料型態組合給予一個比較直觀而易懂的別名
{
    size_t asize;
    unsigned char *a;
    size_t nfuncs;
    hashfunc_t *funcs;
} BloomFilter;

#define SETBIT(a, n) (a[n / CHAR_BIT] |= (1 << (n % CHAR_BIT)))
#define GETBIT(a, n) (a[n / CHAR_BIT] & (1 << (n % CHAR_BIT)))

BloomFilter *bloom_create(size_t size, size_t nfuncs, ...)
{
    BloomFilter *bloom;
    va_list l;
    int n;

    bloom = malloc(sizeof(BloomFilter));
    bloom->a = calloc((size + CHAR_BIT - 1) / CHAR_BIT, sizeof(char));
    bloom->funcs = (hashfunc_t *)malloc(nfuncs * sizeof(hashfunc_t));
    va_start(l, nfuncs);
    for (n = 0; n < nfuncs; ++n)
        bloom->funcs[n] = va_arg(l, hashfunc_t);
    va_end(l);

    bloom->nfuncs = nfuncs;
    bloom->asize = size;

    return bloom;
}

int bloom_add(BloomFilter *bloom, const char *s)
{
    size_t n;

    for (n = 0; n < bloom->nfuncs; ++n)
        SETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize);

    return 0;
}

int bloom_check(BloomFilter *bloom, const char *s)
{
    size_t n;

    for (n = 0; n < bloom->nfuncs; ++n)
    {
        if (!(GETBIT(bloom->a, bloom->funcs[n](s) % bloom->asize)))
            return 0;
    }

    return 1;
}

unsigned int sax_hash(const char *key)
{
    unsigned int h = 0;

    while (*key)
        h ^= (h << 5) + (h >> 2) + (unsigned char)*key++;

    return h;
}

unsigned int sdbm_hash(const char *key)
{
    unsigned int h = 0;
    while (*key)
        h = (unsigned char)*key++ + (h << 6) + (h << 16) - h;
    return h;
}

int main() {     
    BloomFilter* bloom = bloom_create(2500000, 2, sax_hash, sdbm_hash);
    return 0;
}
