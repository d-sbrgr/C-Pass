//
// Created by dave_ on 24.10.2024.
//

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <stdio.h>
#include <string.h>
#include "crypto.h"
#include "util.h"
#include "login.h"


int main() {
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    const char* decrypted_file = "C:/data/input.txt";
    const char* encrypted_file = "C:/data/output.bin";
    char* password = malloc(50 * sizeof(char));

    login(encrypted_file, decrypted_file, password);

    free(password);
    remove(decrypted_file);

    EVP_cleanup();
    ERR_free_strings();
}
