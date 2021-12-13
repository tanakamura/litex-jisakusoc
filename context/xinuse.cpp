#include <stdio.h>
#include <immintrin.h>

__m256i xyzzy;

int main(int argc, char **argv)
{
    auto xcr0 = _xgetbv(0);
    auto xinuse = _xgetbv(1);

    printf("XCR0   : %016llx\n", (long long)xcr0);
    printf("XINUSE : %016llx\n", (long long)xinuse);

    puts("=== use mmx ===");
    _mm_empty();
    xinuse = _xgetbv(1);
    printf("XINUSE : %016llx\n", (long long)xinuse);


    puts("=== use avx ===");
    xyzzy = _mm256_set1_epi8(argc);
    xinuse = _xgetbv(1);
    printf("XINUSE : %016llx\n", (long long)xinuse);
}

