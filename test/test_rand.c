#include "aesrand.h"

#include <sys/time.h>

static double
current_time(void)
{
    struct timeval t;
    (void) gettimeofday(&t, NULL);
    return (double) (t.tv_sec + (double) (t.tv_usec / 1000000.0));
}

static void
print_buf(unsigned char *buf, size_t size)
{
    printf("0x");
    for (size_t i = 0; i < size; ++i) {
        printf("%02x", buf[i]);
    }
    printf("\n");
}

#define NITERS        1000000
#define NITERS_SMALL    10000

int
main(void)
{
    aes_randstate_t rng;
    unsigned char *buf;
    size_t nbits, nbytes;
    double start, end;

    if (aes_randinit(rng) == AESRAND_ERR)
        return 1;

    nbits = 128;

    buf = random_aes(rng, nbits, &nbytes);
    print_buf(buf, nbytes);
    free(buf);

    buf = random_aes(rng, nbits, &nbytes);
    print_buf(buf, nbytes);
    free(buf);

    printf("Testing random_aes: ");
    start = current_time();
    for (int i = 0; i < NITERS; ++i) {
        buf = random_aes(rng, nbits, &nbytes);
        if (buf == NULL)
            return 1;
        free(buf);
    }
    end = current_time();
    printf("%f s\n", end - start);

    printf("Testing mpz_urandomb_aes: ");
    start = current_time();
    {
        mpz_t rop;
        mpz_init(rop);
        for (int i = 0; i < NITERS; ++i) {
            if (mpz_urandomb_aes(rop, rng, 128) == AESRAND_ERR)
                return 1;
        }
        mpz_clear(rop);
    }
    end = current_time();
    printf("%f s\n", end - start);

    printf("Testing mpz_urandomm_aes: ");
    {
        mpz_t rop, n;
        mpz_inits(rop, n, NULL);
        mpz_urandomb_aes(n, rng, 128);
        start = current_time();
        for (int i = 0; i < NITERS; ++i) {
            if (mpz_urandomm_aes(rop, rng, n) == AESRAND_ERR)
                return 1;
        }
        end = current_time();
        mpz_clears(rop, n, NULL);
    }
    printf("%f s\n", end - start);

    printf("Testing mpfr_urandomb_aes: ");
    {
        mpfr_t rop;
        mpfr_init(rop);
        start = current_time();
        for (int i = 0; i < NITERS; ++i) {
            if (mpfr_urandomb_aes(rop, rng) == AESRAND_ERR)
                return 1;
        }
        end = current_time();
        mpfr_clear(rop);
    }
    printf("%f s\n", end - start);

    printf("Testing fmpz_mod_poly_randtest_aes: ");
    {
        fmpz_t q;
        fmpz_mod_poly_t rop;
        mp_bitcnt_t n = 1 << 13;
        fmpz_init(q);
        fmpz_randbits_aes(q, rng, 13);
        fmpz_abs(q, q);
        fmpz_fdiv_q_2exp(q, q, n_flog(n,2)+1);
        fmpz_mul_2exp(q, q, n_flog(n,2)+1);
        fmpz_add_ui(q, q, 1);
        for(int i=0; ; i++) {
            if (fmpz_is_probabprime(q))
                break;
            fmpz_add_ui(q, q, 2*n);
        }
        fmpz_mod_poly_init(rop, q);
        start = current_time();
        for (int i = 0; i < NITERS_SMALL; ++i) {
            if (fmpz_mod_poly_randtest_aes(rop, rng, 64) == AESRAND_ERR)
                return 1;
        }
        end = current_time();
        fmpz_mod_poly_clear(rop);
    }
    printf("%f s\n", end - start);

    printf("Testing fmpz_randbits_aes: ");
    {
        fmpz_t rop;
        fmpz_init(rop);
        start = current_time();
        for (int i = 0; i < NITERS; ++i) {
            if (fmpz_randbits_aes(rop, rng, nbits) == AESRAND_ERR)
                return 1;
        }
        end = current_time();
        fmpz_clear(rop);
    }
    printf("%f s\n", end - start);

    printf("Testing fmpz_randm_aes: ");
    {
        fmpz_t rop, n;
        fmpz_init(rop);
        fmpz_init(n);
        (void) fmpz_randbits_aes(n, rng, nbits);
        start = current_time();
        for (int i = 0; i < NITERS; ++i) {
            if (fmpz_randm_aes(rop, rng, n) == AESRAND_ERR)
                return 1;
        }
        end = current_time();
        fmpz_clear(rop);
        fmpz_clear(n);
    }
    printf("%f s\n", end - start);

    aes_randclear(rng);
    
    return 0;
}
