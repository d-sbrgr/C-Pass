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
#include "util.h"
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
    char** decrypted_char = malloc(sizeof(char *));
    if (!decrypted_char) {
        exit(-99);
    }
    const char* encrypted_file = "c_pass.bin";

    // Display login screen, get master password and decrypt the previously saved passwords
    char* password = login(encrypted_file, decrypted_char);

    // Load the previously saved requirements and passwords from decrypted file
    int num_passwords = 0;
    struct password_requirement* p_requirement = read_password_requirement(*decrypted_char);
    struct password** passwords = read_passwords(*decrypted_char, &num_passwords);

    // Free decrypted characters
    free(*decrypted_char);

    int running = 1;
    clear_console();
    while (running) {
        printf("Choose a option:\n");
        printf("[1] Get a password:\n");
        printf("[2] Generate new password\n");
        printf("[3] Add existing password\n");
        printf("[4] Edit an existing password\n");
        printf("[5] Delete existing password\n");
        printf("[6] Edit password requirements\n");
        printf("[0] Close C-Pass\n");

        int choice;
        int result = scanf("%d", &choice);
        if (result != 1) {
            printf("Invalid input! Please enter an integer.\n");
            while(getchar() != '\n'){}// Clear the buffer
            clear_console();
            continue;
        }
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
            case 4:
                edit_password(passwords, num_passwords, p_requirement);
            break;
            case 5:
                loop_delete_password(&passwords, &num_passwords);
            break;
            case 6:
                update_password_requirements(p_requirement);
            break;
            case 0:
                running = 0;
            break;
            default:
                printf("Invalid choice\n");
        }
    }

    // Save the password requirements and passwords to the decrypted file
    save_passwords_and_requirements(p_requirement, passwords, &num_passwords, decrypted_char);
    free(p_requirement);
    free(passwords);

    // Encrypt the saved data using the master password
    encrypt_file(encrypted_file, decrypted_char, password);
    free(password);
    free(*decrypted_char);
    free(decrypted_char);

    // Cleanup openssl library
    EVP_cleanup();
    ERR_free_strings();
}
