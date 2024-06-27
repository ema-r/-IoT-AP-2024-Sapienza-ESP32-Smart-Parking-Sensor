#include "crypto.h"
static const char * TAG = "CRYPTO_WRAPPER";



void base64stringcat(char **input_array, size_t array_size, char **output_buffer) {
    size_t i, j;
    size_t output_len = 0;
    size_t base64_len = 0;
    unsigned char *base64_buffer = NULL;
    const char delimiter = ':';

    for (i = 0; i < array_size; i++) {
        size_t input_len = strlen(input_array[i]);
        mbedtls_base64_encode(NULL, 0, &base64_len, (unsigned char *)input_array[i], input_len);
        output_len += base64_len;
        if (i < array_size - 1) {
            output_len += 1;
        }
    }

    *output_buffer = (char *)malloc(output_len + 1);  // +1 for the null terminator
    if (*output_buffer == NULL) {
        perror("Failed to allocate memory for output buffer");
        return;
    }
    (*output_buffer)[0] = '\0';

    for (i = 0; i < array_size; i++) {
        size_t input_len = strlen(input_array[i]);
        base64_len = 0;
        mbedtls_base64_encode(NULL, 0, &base64_len, (unsigned char *)input_array[i], input_len);
        base64_buffer = (unsigned char *)malloc(base64_len + 1);
        if (base64_buffer == NULL) {
            perror("Failed to allocate memory for base64 buffer");
            free(*output_buffer);
            return;
        }

        mbedtls_base64_encode(base64_buffer, base64_len, &base64_len, (unsigned char *)input_array[i], input_len);
        base64_buffer[base64_len] = '\0';

        strcat(*output_buffer, (char *)base64_buffer);
        free(base64_buffer);

        if (i < array_size - 1) {
            strncat(*output_buffer, &delimiter, 1);
        }
    }
}



void print_key(mbedtls_pk_context pk, int k){
    unsigned char* buffer=(unsigned char*) malloc(sizeof(unsigned char)*16000);
    int ret;
    if(k==1) ret= mbedtls_pk_write_pubkey_pem(&pk, buffer, 16000-1);
    else ret= mbedtls_pk_write_key_pem(&pk, buffer, 16000-1);
    if(ret!=0){
        printf("Failed to write public key in PEM format\n");
        fflush(stdout);
        return;
    }
    buffer[16000-1]='\0';
    printf("the key is: \n%s\n", buffer);
    free(buffer);
}



void print_exadecimal(const unsigned char* buff, size_t size){
    for(int i=0; i<size; ++i){
        printf("%02x", buff[i]);
    }
    printf("\n");
}


void digital_sign_pem(const unsigned char* message, mbedtls_pk_context pk, size_t* signature_len, unsigned char* sig){

    printf("starting digital signature process\n");
    fflush(stdout);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    const char* pers= "mbedtls_pk_sign\0";
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    int ret= mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char*) pers, strlen(pers));
    if(ret!=0){
        printf("mbedtls drbg seed error\n");
        return;
    }

    unsigned char hash[20];

    ret= mbedtls_md(
        mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),
        message, strlen((char*) message), hash
    );
    print_exadecimal(hash, 20);

    if(ret!=0){
        printf("error in hashing the message for dig. signature\n");
        return;
    }

    size_t sig_len;

    ret= mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA1, hash, 20, sig, MBEDTLS_PK_SIGNATURE_MAX_SIZE ,&sig_len, mbedtls_ctr_drbg_random, &ctr_drbg);

    if(ret!=0){
        printf("error in signin the hashed message, message code: %d\n", ret);
        return;

    }

    printf("generated siganture:\n");

    print_exadecimal(sig, strlen((char*)sig));
    *signature_len= sig_len;
}



mbedtls_pk_context get_local_private_key(const uint8_t* key_char){

    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    printf("the private key is (in char*) and the lenght is%d:\n %s,\n", strlen((const char *)key_char), key_char);
    int ret = mbedtls_pk_parse_key(&pk, (const unsigned char *)key_char, strlen((const char *)key_char)+1, NULL, 0, NULL, 0);
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        printf("failed\n  ! mbedtls_pk_parse_key returned -0x%04x - %s\n\n", -ret, error_buf);
        mbedtls_pk_free(&pk);
    }
    return pk;
}