#include <string.h>
#include <stdlib.h>
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "esp_log.h"
#include "mbedtls/pk.h"
#include "mbedtls/error.h"
#include <mbedtls/oid.h>
#include <mbedtls/asn1.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include "mbedtls/base64.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/error.h"

#include "mbedtls/ecp.h"
#include "esp32/sha.h"



#define ECPARAMS MBEDTLS_ECP_DP_SECP256R1


void print_key(mbedtls_pk_context pk, int k);

void base64stringcat(char **input_array, size_t array_size, char **output_buffer, size_t sig_len);


void print_exadecimal(const unsigned char* buff, size_t size);



int verify_signature(const unsigned char *message, size_t message_len, 
                     const unsigned char *signature, size_t* signature_len, 
                     mbedtls_pk_context *key, mbedtls_ecdsa_context ctx_sign, mbedtls_ecp_point Q, mbedtls_entropy_context entropy, mbedtls_ctr_drbg_context ctr_drbg);



int generate_signature(const char *mess, size_t message_len, 
                        unsigned char *signature, size_t *signature_len, 
                        mbedtls_pk_context *key,  mbedtls_ecdsa_context* res_ctx, mbedtls_ecp_point Q, mbedtls_entropy_context entropy, mbedtls_ctr_drbg_context ctr_drbg);


int load_ecc_private_key(mbedtls_pk_context *pk, const unsigned char* char_key, mbedtls_entropy_context entropy, mbedtls_ctr_drbg_context ctr_drbg);

int init_crypto(mbedtls_ecp_point* Q, mbedtls_entropy_context* entropy, mbedtls_ctr_drbg_context* ctr_drbg);

void free_crypto(mbedtls_ecp_point Q, mbedtls_entropy_context entropy, mbedtls_ctr_drbg_context ctr_drbg);


