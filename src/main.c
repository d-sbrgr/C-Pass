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
#include "password.h"
#include "crypto.h"
#include "login.h"


int main() {
    // Redirect stderr to NUL to suppress all error by openssl
    freopen("NUL", "w", stderr);

    // Initialize the openssl library
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Define file names
    const char* decrypted_file = "c_pass.txt";
    const char* encrypted_file = "c_pass.bin";

    // Allocate memory for the master password
    char* password = malloc(50 * sizeof(char));
    if (!password)
        exit(2);

    // Display login screen, get master password and decrypt the previously saved passwords
    login(encrypted_file, decrypted_file, password);

    // Load the previously saved requirements and passwords from decrypted file
    int num_passwords = 0;
    struct password_requirement* p_requirement = read_password_requirement(decrypted_file);
    struct password** p_passwords = read_passwords(decrypted_file, &num_passwords);

    // Delete decrypted file
    remove(decrypted_file);

    // TODO: add loop where user can interact with passwords

    // Save the password requirements and passwords to the decrytpted file
    save_passwords_and_requirements(p_requirement, p_passwords, &num_passwords, decrypted_file);
    free(p_requirement);
    free(p_passwords);

    // Encrypt the saved data using the master password
    encrypt_file(decrypted_file, encrypted_file, password);
    remove(decrypted_file);
    free(password);

    // Cleanup openssl library
    EVP_cleanup();
    ERR_free_strings();
}
