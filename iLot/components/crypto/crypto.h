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

void digital_sign_pem(const unsigned char* message, mbedtls_pk_context pk, size_t* signature_len, unsigned char* sig);

void print_key(mbedtls_pk_context pk, int k);

void base64stringcat(char **input_array, size_t array_size, char **output_buffer);

mbedtls_pk_context get_local_private_key(const uint8_t* key_char);

void print_exadecimal(const unsigned char* buff, size_t size);
