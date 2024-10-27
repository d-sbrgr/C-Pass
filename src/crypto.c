#include <openssl/applink.c>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include "crypto.h"

#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

/*
 * Derive key and IV from password
 *
 * param const unsigned char* password: Master password set by user
 * param unsigned char* key: Empty char array
 * param unsigned char* iv: Empty char array
 */
bool derive_key_and_iv(const unsigned char *password, unsigned char *key, unsigned char *iv) {
    // Salt is not used for simplicity in this example, but it's recommended in production.
    const unsigned char salt[8] = {0}; // Can use a random salt for security
    int nrounds = 1; // Iterations (adjustable)

    // Using EVP_BytesToKey to derive the key and IV
    if (!EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt, password, strlen((const char *)password), nrounds, key, iv)) {
        fprintf(stderr, "Key derivation failed\n");
        return 0;
    }
    return 1;
}

/*
 * Encrypt data from input file to output file using AES-256-CBC
 *
 * param const char* input_filename: Name of decrypted file
 * param const char* output_filename: Name of encrypted file
 * param char* password: Master password to be used for encryption
 */
bool encrypt_file(const char *input_filename, const char *output_filename, char *password) {
    FILE *input_file = fopen(input_filename, "r");
    FILE *output_file = fopen(output_filename, "wb");

    if (!input_file || !output_file) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    unsigned char key[AES_256_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!derive_key_and_iv((const unsigned char*) password, key, iv)) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    unsigned char buffer[AES_BLOCK_SIZE * 1024];
    unsigned char cipher_buffer[AES_BLOCK_SIZE * 1024 + AES_BLOCK_SIZE];
    int len, cipher_len;

    while ((len = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        if (1 != EVP_EncryptUpdate(ctx, cipher_buffer, &cipher_len, buffer, len)) {
            fclose(input_file);
            fclose(output_file);
            return false;
        }
        fwrite(cipher_buffer, 1, cipher_len, output_file);
    }

    if (1 != EVP_EncryptFinal_ex(ctx, cipher_buffer, &cipher_len)) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }
    fwrite(cipher_buffer, 1, cipher_len, output_file);

    EVP_CIPHER_CTX_free(ctx);
    fclose(input_file);
    fclose(output_file);
    return true;
}

/*
 * Decrypt data from input file to output file using AES-256-CBC
 *
 * param const char* input_filename: Name of encrypted file
 * param const char* output_filename: Name of decrypted file
 * param char* password: Master password to be used for decryption
 */
bool decrypt_file(const char *input_filename, const char *output_filename, char *password) {
    FILE *input_file = fopen(input_filename, "rb");
    FILE *output_file = fopen(output_filename, "w");

    if (!input_file || !output_file) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    unsigned char key[AES_256_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!derive_key_and_iv((const unsigned char*) password, key, iv)) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fclose(input_file);
        fclose(output_file);
        return false;
    }

    unsigned char buffer[AES_BLOCK_SIZE * 1024];
    unsigned char plain_buffer[AES_BLOCK_SIZE * 1024 + AES_BLOCK_SIZE];
    int len, plain_len;

    while ((len = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        if (1 != EVP_DecryptUpdate(ctx, plain_buffer, &plain_len, buffer, len)) {
            fclose(input_file);
            fclose(output_file);
            return false;
        }
        fwrite(plain_buffer, 1, plain_len, output_file);
    }

    if (1 != EVP_DecryptFinal_ex(ctx, plain_buffer, &plain_len)) {
        fprintf(stderr, "Decryption failed\n");
        fclose(input_file);
        fclose(output_file);
        return false;
    }
    fwrite(plain_buffer, 1, plain_len, output_file);

    EVP_CIPHER_CTX_free(ctx);
    fclose(input_file);
    fclose(output_file);
    return true;
}
