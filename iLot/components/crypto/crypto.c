#include "crypto.h"
static const char * TAG = "CRYPTO_WRAPPER";



void base64stringcat(char **input_array, size_t array_size, char **output_buffer, size_t sig_len) {
    size_t i, j;
    size_t output_len = 0;
    size_t base64_len = 0;
    unsigned char *base64_buffer = NULL;
    const char delimiter = ':';

    for (i = 0; i < array_size; i++) {
        size_t input_len;
        if(i==3){
            input_len = sig_len;
        } else {
            input_len = strlen(input_array[i]);
        }
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
        size_t input_len;
        if(i==3){
            input_len = sig_len;
        } else {
            input_len = strlen(input_array[i]);
        }
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

    print_exadecimal(sig, MBEDTLS_PK_SIGNATURE_MAX_SIZE);
    printf("generated signature length %d\n", sig_len);
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


int load_ecc_private_key(mbedtls_pk_context *pk, const uint8_t *key, size_t key_len) {

    return -1;
   
}



int generate_signature(const char *mess, size_t message_len, 
                       unsigned char *signature, size_t *signature_len, 
                       mbedtls_pk_context *key, uint8_t* char_key)
{

    unsigned char* message= *((unsigned char**)(&mess));
    printf("the mess is %s\n", ( char*) message);

    mbedtls_ecdsa_context ctx_sign;
    mbedtls_ecp_point Q;
    mbedtls_ecp_point_init(&Q);
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char hash[32];
    const char *pers = "ecdsa";
    int ret;

    mbedtls_ecdsa_init(&ctx_sign);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    memset(signature, 0, MBEDTLS_ECDSA_MAX_LEN);
     mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char *) pers,
                                     strlen(pers))) != 0) {
        printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        mbedtls_ecdsa_free(&ctx_sign);
        mbedtls_ecp_point_free(&Q);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return -1;
    }

    //here he generates the key, i have to upload it (124-138)
    mbedtls_pk_init(key);
    ret= mbedtls_pk_parse_key(key, char_key, strlen((char*) char_key)+1, NULL, 0, mbedtls_ctr_drbg_random, &ctr_drbg);
    if(ret != 0){
        printf("failed to parse the key\n");
        mbedtls_ecdsa_free(&ctx_sign);
        mbedtls_ecp_point_free(&Q);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return -1;
    }
    printf("the private ec key is:\n");
    print_key(*key, 0);
    print_key(*key, 1);

    ret = mbedtls_ecdsa_from_keypair(&ctx_sign, mbedtls_pk_ec(*key));

    if(ret != 0){
        printf("failed to load the ecdsa key\n");
        mbedtls_ecdsa_free(&ctx_sign);
        mbedtls_ecp_point_free(&Q);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return -1;      
    }


    /*unsigned char h[32];

    printf("message is before hash: %s\n", (char*) message);

    esp_sha(SHA2_256, (unsigned char*)message, message_len, h);
        printf("the hashed esp sha message in hexadecimal is:\n");
    print_exadecimal( h, 32);*/





    printf("  . Computing message hash...");
    fflush(stdout);

    printf("the message in hexadecimal is:\n");
    print_exadecimal( message, message_len);

    if ((ret = mbedtls_sha256(message, message_len, hash, 0)) != 0) {
        printf(" failed\n  ! mbedtls_sha256 returned %d\n", ret);
        mbedtls_ecdsa_free(&ctx_sign);
        mbedtls_ecp_point_free(&Q);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return -1;
    }

    printf(" ok\n");


    print_exadecimal(hash, 32);

    printf("  . Signing message hash...");
    fflush(stdout);

    if ((ret = mbedtls_ecdsa_write_signature(&ctx_sign, MBEDTLS_MD_SHA256,
                                             hash, 32,
                                             signature, MBEDTLS_ECDSA_MAX_LEN, signature_len,
                                             mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
        printf(" failed\n  ! mbedtls_ecdsa_write_signature returned %d\n", ret);
        mbedtls_ecdsa_free(&ctx_sign);
        mbedtls_ecp_point_free(&Q);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return -1;
    }
    printf(" ok (signature length = %u)\n", (unsigned int) *signature_len);







// verification
    mbedtls_ecdsa_context ctx_verify;
    mbedtls_ecdsa_init(&ctx_verify);



    printf("  . Preparing verification context...");
    fflush(stdout);

    mbedtls_ecp_group_id grp_id = mbedtls_ecp_keypair_get_group_id(&ctx_sign);

    if ((ret = mbedtls_ecp_export(&ctx_sign, NULL, NULL, &Q)) != 0) {
        printf(" failed\n  ! mbedtls_ecp_export returned %d\n", ret);
        return -1;
    }

    if ((ret = mbedtls_ecp_set_public_key(grp_id, &ctx_verify, &Q)) != 0) {
        printf(" failed\n  ! mbedtls_ecp_set_public_key returned %d\n", ret);
        return -1;
    }


    printf(" ok\n  . Verifying signature... which has lenght %d\n", *signature_len);
    fflush(stdout);
    printf("the signature to verify is:\n");
    print_exadecimal(signature, *signature_len);



    if ((ret = mbedtls_ecdsa_read_signature(&ctx_verify,
                                            hash, 32,
                                            signature, *signature_len)) != 0) {
        char buf[1024];
        mbedtls_strerror(ret, buf, sizeof(buf));
        printf(" failed\n  ! mbedtls_ecdsa_read_signature returned %s\n", buf);
        return -1;
    } else{
        printf(" the signature is valid!\n");
    }



    mbedtls_ecdsa_free(&ctx_sign);
    mbedtls_ecp_point_free(&Q);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    return 0;

   
}



int verify_signature(const unsigned char *message, size_t message_len, 
                     const unsigned char *signature, size_t signature_len, 
                     mbedtls_pk_context *key)
{
    /*mbedtls_ecdsa_context ctx_verify;
    mbedtls_ecdsa_init(&ctx_verify);



    mbedtls_printf("  . Preparing verification context...");
    fflush(stdout);

    if ((ret = mbedtls_ecp_export(&ctx_sign, NULL, NULL, &Q)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ecp_export returned %d\n", ret);
        goto exit;
    }

    if ((ret = mbedtls_ecp_set_public_key(grp_id, &ctx_verify, &Q)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ecp_set_public_key returned %d\n", ret);
        goto exit;
    }

    */
   /*
     * Verify signature
     */
    /*

    mbedtls_printf(" ok\n  . Verifying signature...");
    fflush(stdout);

    if ((ret = mbedtls_ecdsa_read_signature(&ctx_verify,
                                            hash, sizeof(hash),
                                            sig, sig_len)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ecdsa_read_signature returned %d\n", ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    exit_code = MBEDTLS_EXIT_SUCCESS;

    goto exit;*/

    return -1;
}


