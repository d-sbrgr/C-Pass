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
#include "vault_menu.h"


int main() {
    // Redirect stderr to NUL to suppress all error by openssl
    freopen("NUL", "w", stderr);

    // Initialize the openssl library
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Define file names
    const char* decrypted_file = "c_pass.txt";
    const char* encrypted_file = "c_pass.bin";

    // Display login screen, get master password and decrypt the previously saved passwords
    char* password = login(encrypted_file, decrypted_file);

    // Load the previously saved requirements and passwords from decrypted file
    int num_passwords = 0;
    struct password_requirement* p_requirement = read_password_requirement(decrypted_file);
    struct password** passwords = read_passwords(decrypted_file, &num_passwords);

    // Delete decrypted file
    remove(decrypted_file);

    int running = 1;
    while (running) {
        printf("Choose a option:\n");
        printf("[1] Get a password:\n");
        printf("[2] Generate new password\n");
        printf("[3] Add existing password\n");
        printf("[0] Close C-Pass\n");

        int choice;
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                get_password(passwords, num_passwords);
            break;
            case 2:
                generate_and_save_password(passwords, &num_passwords, p_requirement);
            break;
            case 3:
                add_existing_password(passwords, &num_passwords, p_requirement);
            break;
            case 0:
                running = 0;
            break;
            default:
                printf("Invalid choice\n");
        }
    }

    // Save the password requirements and passwords to the decrypted file
    save_passwords_and_requirements(p_requirement, passwords, &num_passwords, decrypted_file);
    free(p_requirement);
    free(passwords);

    // Encrypt the saved data using the master password
    encrypt_file(decrypted_file, encrypted_file, password);
    remove(decrypted_file);
    free(password);

    // Cleanup openssl library
    EVP_cleanup();
    ERR_free_strings();
}
