#include "simon.h"

static void _simon128_enc_round(key128_t round_key,
                                block128_t input, block128_t* output) {

}

static void _simon128_dec_round(key128_t round_key, block128_t input,
                                block128_t *output) {
}

void simon128_encrypt(key128_t key,
                      block128_t* input, block128_t* output,
                      uint32_t block_count) {

}

void simon128_decrypt(key128_t key,
                      block128_t* input, block128_t* output,
                      uint32_t block_count) {

}
