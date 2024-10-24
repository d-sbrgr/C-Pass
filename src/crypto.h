//
// Created by dave_ on 24.10.2024.
//

#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdbool.h>

bool encrypt_file(const char *input_filename, const char *output_filename, char *password);
bool decrypt_file(const char *input_filename, const char *output_filename, char *password);


#endif //CRYPTO_H
