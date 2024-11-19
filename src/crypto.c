#ifdef _WIN32
#include <openssl/applink.c>
#endif
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
bool encrypt_file(const char *encrypted_filename, char **input, char *password) {
    FILE *output_file = fopen(encrypted_filename, "wb");

    if (!output_file) {
        fclose(output_file);
        return false;
    }

    unsigned char key[AES_256_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!derive_key_and_iv((const unsigned char*) password, key, iv)) {
        fclose(output_file);
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx){
        fclose(output_file);
        return false;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fclose(output_file);
        return false;
    }

    const size_t chunk_size = AES_BLOCK_SIZE * 1024;  // Define chunk size
    unsigned char buffer[chunk_size];
    unsigned char cipher_buffer[chunk_size + AES_BLOCK_SIZE];
    int cipher_len;

    size_t input_size = strlen(*input);
    size_t offset = 0;
    while (offset < input_size) {
        size_t bytes_to_read = input_size - offset > chunk_size ? chunk_size : input_size - offset;

        // Copy data from the input array into the buffer
        memcpy(buffer, *input + offset, bytes_to_read);

        // Encrypt the current chunk
        if (1 != EVP_EncryptUpdate(ctx, cipher_buffer, &cipher_len, buffer, bytes_to_read)) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(output_file);
            return false;
        }

        // Write the encrypted chunk to the output file
        if (fwrite(cipher_buffer, 1, cipher_len, output_file) != cipher_len) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(output_file);
            return false;
        }

        offset += bytes_to_read;
    }

    // Finalize the encryption
    if (1 != EVP_EncryptFinal_ex(ctx, cipher_buffer, &cipher_len)) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(output_file);
        return false;
    }

    if (fwrite(cipher_buffer, 1, cipher_len, output_file) != cipher_len) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(output_file);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);
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
bool decrypt_file(const char *encrypted_filename, char **output, char *password) {
    FILE *input_file = fopen(encrypted_filename, "rb");

    if (!input_file) {
        fclose(input_file);
        return false;
    }

    unsigned char key[AES_256_KEY_SIZE], iv[AES_BLOCK_SIZE];
    if (!derive_key_and_iv((const unsigned char*) password, key, iv)) {
        fclose(input_file);
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fclose(input_file);
        return false;
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        fclose(input_file);
        return false;
    }

    unsigned char buffer[AES_BLOCK_SIZE * 1024];
    int len, plain_len;
    // Allocate initial memory for output
    size_t total_size = 0;
    size_t allocated_size = AES_BLOCK_SIZE * 1024;
    *output = calloc(allocated_size, sizeof(char));
    if (!*output) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(input_file);
        return false;
    }

    while ((len = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        if (1 != EVP_DecryptUpdate(ctx, (unsigned char *)*output + total_size, &plain_len, buffer, len)) {
            free(*output);
            EVP_CIPHER_CTX_free(ctx);
            fclose(input_file);
            return false;
        }
        total_size += plain_len;

        // Resize output buffer if needed
        if (total_size + AES_BLOCK_SIZE > allocated_size) {
            allocated_size *= 2;
            char *new_output = realloc(*output, allocated_size);
            if (!new_output) {
                free(*output);
                EVP_CIPHER_CTX_free(ctx);
                fclose(input_file);
                return false;
            }
            *output = new_output;
        }
    }

    if (1 != EVP_DecryptFinal_ex(ctx, (unsigned char *)*output + total_size, &plain_len)) {
        fprintf(stderr, "Decryption failed\n");
        free(*output);
        EVP_CIPHER_CTX_free(ctx);
        fclose(input_file);
        return false;
    }
    total_size += plain_len;

    // Resize to actual size
    char *final_output = realloc(*output, total_size + 1);
    if (final_output) {
        final_output[total_size] = '\0';
        *output = final_output;
    }

    EVP_CIPHER_CTX_free(ctx);
    fclose(input_file);
    return true;
}
