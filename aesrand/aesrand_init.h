#ifndef _AESRAND_INIT_H_
#define _AESRAND_INIT_H_

#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#define AESRAND_OK  0
#define AESRAND_ERR (-1)

struct _aes_randstate_struct {
    char aes_init;
    unsigned long ctr;
    unsigned char key[128];
};

typedef struct _aes_randstate_struct aes_randstate_t[1];

#define AES_ALGORITHM EVP_aes_128_ctr()

int  aes_randinit(aes_randstate_t state); // seed from /dev/urandom
void aes_randinit_seedn(aes_randstate_t state, char *seed, size_t seed_size,
                        char *additional, size_t additional_size);
void aes_randclear(aes_randstate_t state);
int  aes_randstate_fwrite(const aes_randstate_t state, FILE *fp);
int  aes_randstate_fread(aes_randstate_t state, FILE *fp);
int  aes_randstate_write(const aes_randstate_t state, const char *fname);
int  aes_randstate_read(aes_randstate_t state, const char *fname);

#endif
