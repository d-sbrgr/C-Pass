//
// Created by Nicola Hermann on 18.11.2024.
//

#include "vault_menu.h"
#include <stdio.h>
#include "password.h"
#include <string.h>
#include <stdlib.h>

void get_password(struct password **p_passwords, const int num_passwords) {
    char name[256];
    printf("---Get a password---\n");
    printf("Enter website / application name\n");
    scanf("%255s", name);  // Use %255s to prevent buffer overflow

    // Consume any leftover newline from previous input
    while(getchar() != '\n');

    for (int i = 0; i < num_passwords; i++) {
        if (p_passwords[i] != NULL && strcmp(p_passwords[i]->name, name) == 0) {
            // If the name matches, display the stored username and password
            printf("Username for %s: %s\n", p_passwords[i]->name, p_passwords[i]->username);
            printf("Password: %s\n", p_passwords[i]->password);
            printf("--------------\n");
            return;  // Exit the function once the password is found
        }
    }
    // If no match is found
    printf("No password found for %s.\n", name);
    printf("--------------\n");
}

void generate_and_save_password(
    struct password **p_passwords,
    int *p_num_passwords,
    const struct password_requirement *requirements) {
    char name[256], username[256];
    printf("---Generate new password---\n");
    printf("Enter website / application name: \n");
    scanf("%255s", name);
    printf("Enter username: \n");
    scanf("%255s", username);

    char *new_password = generate_password(requirements);
    if (!new_password) {
        printf("Failed to generate password. Ensure requirements are valid.\n");
        printf("--------------\n");
        return;
    }

    printf("Generated password: %s\n", new_password);
    add_password(&p_passwords, p_num_passwords, name, username, new_password);
    free(new_password);

    printf("Password saved successfully.\n");
    printf("--------------\n");
}

void add_existing_password(struct password **p_passwords, int *p_num_passwords, const struct password_requirement *requirements) {
    char name[256], username[256], password[256];
    printf("---Add existing password ---\n");
    printf("Enter website / application name: \n");
    scanf("%255s", name);

    // Consume leftover newline character
    while (getchar() != '\n');

    printf("Enter the username for this site: \n");
    scanf("%255s", username);

    // Consume leftover newline character
    while (getchar() != '\n');

    while(1) {
        printf("Enter the password for this site (or leave blank to exit): \n");
        scanf("%255s", password);

        if (strlen(password) == 0) {
            printf("No password provided. Exiting.\n");
            return;
        }

        if (is_valid_password(password, requirements)) {
            add_password(&p_passwords, p_num_passwords, name, username, password);
            printf("Password successfully added for %s!\n", name);
            printf("--------------\n");
            break; // Exit the loop after successful addition
        }

        printf("Password does not meet the requirements. Try again.\n");

    }

}

