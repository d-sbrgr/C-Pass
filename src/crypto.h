//
// Created by dave_ on 24.10.2024.
//

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdbool.h>

bool encrypt_file(const char *encrypted_filename, char **input, char *password);
bool decrypt_file(const char *encrypted_filename, char **output, char *password);


#endif //CRYPTO_H
