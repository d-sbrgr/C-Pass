//
// Created by Nicola Hermann on 18.11.2024.
//

#include "vault_menu.h"
#include <stdio.h>
#include "password.h"
#include <string.h>
#include <stdlib.h>

#include "util.h"

void get_password(struct password **p_passwords, const int num_passwords) {

    char name[256];
    clear_console();
    printf("---Get a password---\n");
    list_password_names(p_passwords, &num_passwords);
    printf("Enter website / application name\n");
    scanf("%255s", name);  // Use %255s to prevent buffer overflow

    // Consume any leftover newline from previous input
    while(getchar() != '\n');

    for (int i = 0; i < num_passwords; i++) {
        if (p_passwords[i] != NULL && strcmp(p_passwords[i]->name, name) == 0) {
            // If the name matches, display the stored username and password
            clear_console();
            printf("Username for %s: %s\n", p_passwords[i]->name, p_passwords[i]->username);
            printf("Password: %s\n", p_passwords[i]->password);
            printf("--------------\n");
            return;  // Exit the function once the password is found
        }
    }
    // If no match is found
    clear_console();
    printf("No password found for %s.\n", name);
    printf("--------------\n");
}

void generate_and_save_password(
    struct password **p_passwords,
    int *p_num_passwords,
    const struct password_requirement *requirements) {
    char name[256], username[256];
    clear_console();
    printf("---Generate new password---\n");
    printf("Enter website / application name: \n");
    scanf("%255s", name);
    printf("Enter username: \n");
    scanf("%255s", username);

    char *new_password = generate_password(requirements);
    if (!new_password) {
        clear_console();
        printf("Failed to generate password. Ensure requirements are valid.\n");
        printf("--------------\n");
        return;
    }
    clear_console();
    printf("Generated password: %s\n", new_password);
    printf("--------------\n");
    add_password(&p_passwords, p_num_passwords, name, username, new_password);
    free(new_password);
}

void add_existing_password(struct password **p_passwords, int *p_num_passwords, const struct password_requirement *requirements) {
    char name[256], username[256], password[256];
    clear_console();
    printf("---Add existing password ---\n");
    printf("Enter website / application name: \n");
    scanf("%255s", name);

    // Consume leftover newline character
    while (getchar() != '\n') {}

    printf("Enter the username for this site: \n");
    scanf("%255s", username);

    // Consume leftover newline character
    while (getchar() != '\n');

    while(1) {
        printf("Enter the password for this site (or leave blank to exit): \n");
        scanf("%255s", password);

        if (strlen(password) == 0) {
            clear_console();
            printf("No password provided. Exiting.\n");
            printf("--------------\n");
            return;
        }

        if (is_valid_password(password, requirements)) {
            add_password(&p_passwords, p_num_passwords, name, username, password);
            clear_console();
            printf("Password successfully added for %s!\n", name);
            printf("--------------\n");
            break; // Exit the loop after successful addition
        }
        printf("Password does not meet the requirements. Try again.\n");
    }
}

void list_password_names(struct password** passwords, const int *num_passwords) {
    if (*num_passwords == 0) {
        printf("No passwords saved yet.\n");
        return;
    }

    printf("Available passwords:\n");
    for (int i = 0; i < *num_passwords; i++) {
        printf("[%d] %s\n", i + 1, passwords[i]->name);
    }
}

void edit_password(struct password** passwords, int num_passwords, const struct password_requirement *requirements) {
    clear_console();
    printf("---Edit password ---\n");
    list_password_names(passwords, &num_passwords);

    int choice;
    printf("Enter your choice (1-%d): ", num_passwords);
    int result = scanf("%d", &choice);
    if (result != 1) {
        while(getchar() != '\n'){}
        clear_console();
        printf("Invalid choice.\n");
        printf("--------------\n");
        return;
    }

    if (choice < 1 || choice > num_passwords) {
        clear_console();
        printf("Invalid choice.\n");
        printf("--------------\n");
        return;
    }

    struct password* selected_password = passwords[choice - 1];

    printf("Editing password for '%s':\n", selected_password->name);

    char new_username[100];
    char new_password[100];

    printf("Enter new username (or press Enter to keep '%s'): ", selected_password->username);
    getchar();
    fgets(new_username, sizeof(new_username), stdin);
    new_username[strcspn(new_username, "\n")] = '\0'; // Remove trailing newline

    // Update username if not empty
    printf("Enter new password (or press Enter to keep the current password, or type 'generate' to generate a new password): ");
    fgets(new_password, sizeof(new_password), stdin);
    new_password[strcspn(new_password, "\n")] = '\0'; // Remove trailing newline

    if (strcmp(new_password, "generate") == 0) {
        // Call a password generation function
        char *generated_password = generate_password(requirements);
        if (generated_password) {
            strcpy(new_password, generated_password);
            free(generated_password);
        } else {
            printf("Password generation failed due to invalid requirements.\n");
            free(generated_password);
            return;
        }
    }
    clear_console();
    // Update username if not empty
    if (strlen(new_username) > 0) {
        free(selected_password->username);
        selected_password->username = strdup(new_username);
        printf("Successfully updated username to: '%s'\n",new_username);
    }

    // Update password if not empty
    if (strlen(new_password) > 0) {
        free(selected_password->password);
        selected_password->password = strdup(new_password);
        printf("Successfully updated password to: '%s'\n",new_password);
    }


    printf("--------------\n");
}

void loop_delete_password(struct password*** passwords, int* num_passwords) {
    clear_console();
    printf("---Delete password ---\n");
    list_password_names(*passwords, num_passwords);

    int choice;
    printf("Enter your choice (1-%d): ", *num_passwords);
    const int result = scanf("%d", &choice);
    if (result != 1) {
        while(getchar() != '\n'){}
        clear_console();
        printf("Invalid choice.\n");
        printf("--------------\n");
        return;
    }

    if (choice < 1 || choice > *num_passwords) {
        clear_console();
        printf("Invalid choice.\n");
        printf("--------------\n");
        return;
    }
    delete_password(passwords, choice - 1);

    for (int i = choice; i < *num_passwords; i++) {
        (*passwords)[i - 1] = (*passwords)[i];
    }
    (*num_passwords)--;

    // Adjust the size of the array
    *passwords = realloc(*passwords, (*num_passwords) * sizeof(struct password*));
    if (*passwords == NULL && *num_passwords > 0) {
        perror("Error reallocating memory");
        exit(EXIT_FAILURE);
    }
    clear_console();
    printf("Password deleted successfully.\n");
    printf("--------------\n");
}

void update_password_requirements(struct password_requirement* req) {
    clear_console();
    printf("Current password requirements:\n");
    printf("1. Minimum length: %d\n", req->length);
    printf("2. Minimum uppercase letters: %d\n", req->uppercased);
    printf("3. Minimum digits: %d\n", req->digits);
    printf("4. Minimum special characters: %d\n", req->special_characters);

    printf("\nEnter new requirements (enter -1 to keep current value):\n");

    int new_value;

    // Update minimum length
    printf("1. Minimum length: ");
    scanf("%d", &new_value);
    if (new_value >= 0) {
        req->length = new_value;
    }

    // Update minimum uppercase letters
    printf("2. Minimum uppercase letters: ");
    scanf("%d", &new_value);
    if (new_value >= 0) {
        req->uppercased = new_value;
    }

    // Update minimum digits
    printf("3. Minimum digits: ");
    scanf("%d", &new_value);
    if (new_value >= 0) {
        req->digits = new_value;
    }

    // Update minimum special characters
    printf("4. Minimum special characters: ");
    scanf("%d", &new_value);
    if (new_value >= 0) {
        req->special_characters = new_value;
    }
    clear_console();
    printf("\nUpdated password requirements:\n");
    printf("1. Minimum length: %d\n", req->length);
    printf("2. Minimum uppercase letters: %d\n", req->uppercased);
    printf("3. Minimum digits: %d\n", req->digits);
    printf("4. Minimum special characters: %d\n", req->special_characters);
    printf("--------------\n");
}
