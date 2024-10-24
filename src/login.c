//
// Created by dave_ on 24.10.2024.
//

#include "login.h"
#include "util.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>


void print_application_header(void){
    printf("  ______        .______      ___           _______.     _______.\n");
    printf(" /      |       |   _  \\    /   \\         /       |    /       |\n");
    printf("|  ,----' ______|  |_)  |  /  ^  \\       |   (----`   |   (----`\n");
    printf("|  |     |______|   ___/  /  /_\\  \\       \\   \\        \\   \\\n");
    printf("|  `----.       |  |     /  _____  \\  .----)   |   .----)   |\n");
    printf(" \\______|       | _|    /__/     \\__\\ |_______/    |_______/   \n\n");
    printf("==================================================================\n\n");
}

void print_welcome_message(void){
  printf("Welcome to C-Pass, the console based password manager\n\n");
}


void login_dialog(char* password, const int remaining_tries, const bool first_time_user) {
    system("cls");
    print_application_header();
    if (first_time_user)
        print_welcome_message();
    if (remaining_tries)
        printf("%i tries remaining\n", remaining_tries);
    printf("Enter your password: (max 50 characters)\n");
    scanf("%50s", password);
}

void login(const char* encrypted_file, const char* decrypted_file, char* password) {
    if (file_exists(encrypted_file)) {
        int i = 3;
        for (; i > 0; i--) {
            login_dialog(password, i, false);
            if (decrypt_file(encrypted_file, decrypted_file, password))
                break;
        }
        if (!i)
            exit(1);
    } else {
        login_dialog(password, 0, true);
    }
}