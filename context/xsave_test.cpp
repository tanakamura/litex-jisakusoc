#include <cpuid.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

extern "C" {
void touch_avx();
void touch_x87();
void touch_mmx();
void do_vzeroupper();
void do_vzeroall();
void do_emms();
void do_xsave(char *p, uint64_t instruction_mask);
void do_xsaveopt(char *p, uint64_t instruction_mask);
void do_xsavec(char *p, uint64_t instruction_mask);
void do_xrstor(char *p, uint64_t instruction_mask);

void set_mm0(void*p);
void set_xmm0(void*p);
void set_ymm0(void*p);
void set_zmm0(void*p);

}

struct sse_reg {
    union {
        uint8_t b8[16];
        uint32_t b32[4];
        uint64_t b64[2];
    };
};
struct avx_reg {
    union {
        uint8_t b8[32];
        uint32_t b32[8];
        uint64_t b64[4];
    };
};
struct avx512_reg {
    union {
        uint8_t b8[64];
        uint32_t b32[16];
        uint64_t b64[8];
    };
};

#define FOR_EACH_STATE_BIT(F)    \
    F(X87, 0, true)              \
    F(SSE, 1, true)              \
    F(AVX, 2, true)              \
    F(MPX_BNDREGS, 3, true)      \
    F(MPX_BNDCSR, 4, true)       \
    F(AVX512_OPMASK, 5, true)    \
    F(AVX512_ZMM_Hi256, 6, true) \
    F(AVX512_Hi16_ZMM, 7, true)  \
    F(PT, 8, false)              \
    F(PKRU, 9, true)             \
    F(CET_U, 11, false)          \
    F(CET_S, 12, false)          \
    F(HDC, 13, false)            \
    F(HWP, 16, false)            \
    F(XTILECFG, 17, true)        \
    F(XTILEDATA, 18, true)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define DEF_ENUM(NAME, BIT, USER) NAME,

enum class STATE_COMPONENT {
    FOR_EACH_STATE_BIT(DEF_ENUM)

        NUM_STATE_COMPONENT
};

#define DEF_STRUCT(NAME, BIT, USER) {#NAME, BIT, USER},

struct StateComponent {
    std::string name;
    int bitpos;
    int bytepos;
    int bytesize;
    bool user;
    bool available = false;

    StateComponent(std::string const &name, int bitpos, bool user)
        : name(name), bitpos(bitpos), user(user) {}
};

std::vector<StateComponent> state_components{FOR_EACH_STATE_BIT(DEF_STRUCT)};
static int xsave_fullsize;
static uint64_t state_fullmask;

static void init_offset() {
    uint64_t xcr0 = _xgetbv(0);
    state_fullmask = xcr0;
    for (auto &&e : state_components) {
        if (xcr0 & (1ULL << e.bitpos)) {
            unsigned int eax, ebx, ecx, edx;
            __cpuid_count(0x0d, e.bitpos, eax, ebx, ecx, edx);

            if (e.bitpos == 0) {
                // x87
                xsave_fullsize = ebx;
                e.bytepos = 0;
                e.bytesize = 160;
                e.available = true;
            } else if (e.bitpos == 1) {
                // sse
                e.bytepos = 160;
                e.bytesize = 16 * 16;
                e.available = true;
            } else {
                e.bytepos = ebx;
                e.bytesize = eax;
                e.available = true;
            }
        }
    }

    printf("xsave context size = %d\n", xsave_fullsize);

    for (auto &&e : state_components) {
        if (e.available) {
            printf("%16s (%4d): offset=%d, size=%d\n", e.name.c_str(), e.bitpos,
                   e.bytepos, e.bytesize);
        }
    }
}

static void dump_xinuse() {
    uint64_t v = _xgetbv(1);
    printf("XINUSE    : %016llx\n", (long long)v);
}

static void dump_xheader(char *p) {
    long long *p64 = (long long*)(p + 512);

    printf("XSTATE_BV : %016llx\n", p64[0]);
    printf("XCOMP_BV  : %016llx\n", p64[1]);
}

static void set_xheader(char *p, uint64_t state_bv, uint64_t comp_bv) {
    long long *p64 = (long long*)(p + 512);

    p64[0] = state_bv;
    p64[1] = comp_bv;
}

char mem64[64] __attribute__((aligned(64)));

int main() {
    init_offset();

    auto p = new (std::align_val_t(64)) char[xsave_fullsize];
    auto empty_state = new (std::align_val_t(64)) char[xsave_fullsize];

    memset(p, 0, xsave_fullsize);
    memset(empty_state, 0, xsave_fullsize);
    *(uint32_t*)&empty_state[24] = 0x00001f80;  // MXCSR

    set_xheader(p, 0, 0);
    set_xheader(empty_state, 0, 0);

    /* clear XINUSE */
    do_xrstor(empty_state, state_fullmask);
    dump_xinuse();

    set_mm0(mem64);
    dump_xinuse();

    delete [] p;
    delete [] empty_state;
}
