#include <asm/unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall.h>
#include <string.h>
#include <unistd.h>

int putchar(int c) {
    char p = c;
    syscall(__NR_write, 1, &p, 1);
    return 0;
}

int puts(const char *s) {
    while (*s) {
        putchar(*s);
        s++;
    }
    putchar('\n');
    return 0;
}

template <typename IT, typename LT, typename LLT, typename VT, int base, bool SIGN>
void show_number(bool have_zero, int long_count, int col, va_list ap) {
    VT val;
    if (long_count == 0) {
        val = va_arg(ap, IT);
    } else if (long_count == 1) {
        val = va_arg(ap, LT);
    } else {
        val = va_arg(ap, LLT);
    }

    int num_digit = 0;
    VT divider = 1;

    if (val == 0) {
        num_digit = 1;
    } else {
        VT copy = val;
        while (copy) {
            copy /= base;
            if (num_digit != 0) {
                divider *= base;
            }
            num_digit++;
        }
    }

    if (SIGN) {
        if (val < 0) {
            num_digit++;
            val = -val;  // TODO: INT_MIN
            putchar('-');
        }
    }

    if (col > num_digit) {
        int rem = col - num_digit;
        for (int i = 0; i < rem; i++) {
            if (have_zero) {
                putchar('0');
            } else {
                putchar(' ');
            }
        }
    }
    if (val == 0) {
        putchar('0');
    } else {
        while (divider) {
            if (base == 10) {
                putchar(((val / divider) % base) + '0');
            } else if (base == 16) {
                static const char hex_table[] = "0123456789abcdef";
                putchar(hex_table[(val / divider) % base]);
            }
            divider /= base;
        }
    }
}

int vprintf(const char *__restrict__ format, va_list ap) {
    int ret = 0;

    while (1) {
        char c = *(format++);
        if (!c) {
            break;
        }
        if (c == '%') {
            bool output_done = false;
            bool have_zero = false;
            int long_count = 0;
            int col = 0;
            while (!output_done) {
                char c2 = *(format++);

                switch (c2) {
                    case 'd':
                        show_number<int, long, long long, int64_t, 10, true>(
                            have_zero, long_count, col, ap);
                        output_done = true;
                        break;
                    case 'u':
                        show_number<unsigned int, unsigned long,
                                    unsigned long long, uint64_t, 10, true>(
                            have_zero, long_count, col, ap);
                        output_done = true;
                        break;
                    case 'x':
                        show_number<unsigned int, unsigned long,
                                    unsigned long long, uint64_t, 16, true>(
                            have_zero, long_count, col, ap);
                        output_done = true;
                        break;

                    case 'l':
                        long_count++;
                        break;

                    case '0':
                        have_zero = true;
                        break;

                    case 's': {
                        char *p = va_arg(ap, char*);
                        size_t len = strlen(p);
                        size_t dcol = col - len;
                        for (size_t c=0; c<dcol; c++) {
                            putchar(' ');
                        }
                        while (*p) {
                            putchar(*p);
                            p++;
                        }
                        output_done = true;
                        break;
                    }break;

                    default:
                        if (c2 >= '1' && c2 <= '9') {
                            col = 0;
                            format--;
                            while (c2 >= '0' && c2 <= '9') {
                                col *= 10;
                                col += c2 - '0';
                                format++;
                                c2 = *format;
                            }
                        }
                        break;
                }
            }
            ret++;
        } else {
            putchar(c);
        }
    }

    return ret;
}

int printf(const char *__restrict__ format, ...) {
    va_list ap;
    va_start(ap, format);
    int ret = vprintf(format, ap);
    va_end(ap);
    return ret;
}

void *memset(void *s, int c, size_t n) {
    char *p = (char *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = c;
    }

    return p + n;
}

size_t strlen(const char *s) {
    size_t ret = 0;
    while (s[ret]) {
        ret++;
    }

    return ret;
}
