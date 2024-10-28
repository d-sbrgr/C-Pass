//
// Created by dave_ on 24.10.2024.
//

#include "login.h"
#include "password.h"
#include "util.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>


/*
 * Print the ASCII-art header of the application
 */
void print_application_header(void){
    printf("  ______        .______      ___           _______.     _______.\n");
    printf(" /      |       |   _  \\    /   \\         /       |    /       |\n");
    printf("|  ,----' ______|  |_)  |  /  ^  \\       |   (----`   |   (----`\n");
    printf("|  |     |______|   ___/  /  /_\\  \\       \\   \\        \\   \\\n");
    printf("|  `----.       |  |     /  _____  \\  .----)   |   .----)   |\n");
    printf(" \\______|       | _|    /__/     \\__\\ |_______/    |_______/   \n\n");
    printf("==================================================================\n\n");
}

/*
 * Print welcome message for new users
 */
void print_welcome_message(void){
  printf("Welcome to C-Pass, the console based password manager\n\n");
}

/*
 * Display login dialog when first starting the application
 *
 * param const int remaining_tries: Remaining tries to enter correct password until application is stopped
 * param const bool first_time_user: Depicting whether the application has been started before
 * return char*: Password entered by the user
 */
char* login_dialog(const int remaining_tries, const bool first_time_user) {
    system("cls");
    print_application_header();
    if (first_time_user)
        print_welcome_message();
    if (remaining_tries)
        printf("%i tries remaining\n", remaining_tries);
    return read_password();
}

/*
 * The login loop when starting the application. The maximum amount of tries to enter the correct password
 * until the application is shut down is 3.
 *
 * param const char* encrypted_file: The file name of the encrypted file storing the saved passwords
 * param const char* decrypted_file: The file name where the decrypted contents should be saved
 * return char*: The master password as character array
 */
char* login(const char* encrypted_file, const char* decrypted_file) {
    if (file_exists(encrypted_file)) {
        int i = 3;
        for (; i > 0; i--) {
            char* password = login_dialog(i, false);
            if (decrypt_file(encrypted_file, decrypted_file, password))
                return password;
            free(password);
        }
        if (!i)
            exit(1);
    }
    return login_dialog(0, true);
}
