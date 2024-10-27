//
// Created by dave_ on 27.10.2024.
//

#include "password.h"

#include <stdio.h>

#include "util.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>


const char *digits = "0123456789";
const char *special_characters = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";
const char *alpha_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *alpha_lower = "abcdefghijklmnopqrstuvwxyz";

/*
Generate a random new password that matches the requirements

param struct password_requirement* requirement: Pointer to the minimum password requirement defined by the user
return char*: Newly created character array with the new password
 */
char* generate_password(const struct password_requirement* requirement) {
    if (requirement->length < requirement->uppercased + requirement->digits + requirement->special_characters) {
      return NULL;
    }
    // Seed the random number generator
    char* password = calloc(requirement->length, sizeof(char));

    srand((unsigned int)time(NULL));

    int i = 0;

    // Add required number of digits
    for (int j = 0; j < requirement->digits; j++) {
        password[i++] = digits[rand() % strlen(digits)];
    }

    // Add required number of special characters
    for (int j = 0; j < requirement->special_characters; j++) {
        password[i++] = special_characters[rand() % strlen(special_characters)];
    }

    // Add required number of uppercased letter
    for (int j = 0; j < requirement->uppercased; j++) {
        password[i++] = alpha_upper[rand() % strlen(alpha_upper)];
    }

    // Fill the remaining characters with alphabetic characters
    while (i < requirement->length) {
        password[i++] = alpha_lower[rand() % strlen(alpha_lower)];
    }

    // Shuffle the password to mix the characters
    for (int j = 0; j < requirement->length; j++) {
        const int k = rand() % requirement->length;
        if (j != k) {
            const char temp = password[j];
            password[j] = password[k];
            password[k] = temp;
        }
    }

    // Null-terminate the password
    password[requirement->length] = '\0';
    return password;
}


void delete_password(struct password*** arr, const int* index) {
    if (*arr == NULL)
        return;
    struct password* temp = (*arr)[*index];
    free(temp->name);
    free(temp->username);
    free(temp->password);
    free(temp);
    (*arr)[*index] = NULL;
}


void change_password(struct password*** arr, const int* index, const char* password) {
    if (*arr == NULL)
        return;
    struct password* temp = (*arr)[*index];
    free(temp->password);
    temp->password = strdup(password);
}


void add_password(
    struct password ***arr,
    int *curr_size,
    const char *name,
    const char* username,
    const char *password) {
    if (*curr_size % DEFAULT_CAPACITY == 0 && *curr_size > 0) {
        struct password **new_array = realloc(*arr, *curr_size + DEFAULT_CAPACITY * sizeof(struct password*));
        if (!new_array) {
            perror("Failed to resize array");
            return;
        }
        *arr = new_array;
        struct password** ptr = (*arr) + *curr_size;
        // Initialize newly allocated pointers to NULL
        for (size_t i = *curr_size; i < *curr_size + DEFAULT_CAPACITY; i++, ptr++) {
            *ptr = NULL;
        }
    }

    struct password** ptr = (*arr) + *curr_size;

    // Allocate memory for the new password entry
    *ptr = calloc(1, sizeof(struct password));
    (*ptr)->name = strdup(name);
    (*ptr)->username = strdup(username);
    (*ptr)->password = strdup(password);
    (*curr_size)++;
}


struct password ** read_passwords(const char *file_name, int* curr_size) {
    struct password** p_passwords = calloc(DEFAULT_CAPACITY, sizeof(struct password*));
    *curr_size = 0;
    // If there are no passwords yet, return empty array
    if (!file_exists(file_name)){
        return p_passwords;
    }
    FILE* file = fopen(file_name, "r");
    char buffer[1024];

    // Skip the first line, because this is the password requirement
    fgets(buffer, sizeof(buffer), file);

    // Read and process remaining lines and store the passwords.
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        const char* name = strtok(NULL, " ");
        const char* username = strtok(NULL, " ");
        const char* password = strtok(NULL, " ");
        add_password(&p_passwords, curr_size, name, username, password);
    }

    fclose(file);
    return p_passwords;
}


/*
Read the password requirement saved in the password file. If none has been defined yet, a default is returned.

param const char* file_name: Decrypted file, where passwords are stored
return struct password_requirement*: Pointer to the struct containing the defined password requirements
*/
struct password_requirement* read_password_requirement(const char* file_name) {
    struct password_requirement* p_requirement = calloc(1, sizeof(struct password_requirement));
    p_requirement->length = 12;
    p_requirement->uppercased = 1;
    p_requirement->digits = 1;
    p_requirement->special_characters = 1;

    // If there is no requirement set, return default requirement
    if (!file_exists(file_name)){
        return p_requirement;
    }

    FILE* file = fopen(file_name, "r");
    char line[256];
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return p_requirement;
    }
    fclose(file);

    // Parse the line for integers separated by spaces
    const char *token = strtok(line, " ");
    if (token) p_requirement->length = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->uppercased = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->digits = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->special_characters = atoi(token);
    return p_requirement;
}


void save_password_requirement(struct password_requirement* requirement, FILE* file) {
    char buffer[50];
    const int len_length = snprintf(buffer, sizeof(buffer), "%d", requirement->length);
    const int len_digits = snprintf(buffer, sizeof(buffer), "%d", requirement->digits);
    const int len_uppercased = snprintf(buffer, sizeof(buffer), "%d", requirement->uppercased);
    const int len_special = snprintf(buffer, sizeof(buffer), "%d", requirement->special_characters);

    // Total length: sum of lengths + 3 spaces + 1 for '\0'
    const size_t total_length = len_length + len_digits + len_uppercased + len_special + 4;

    char *result = malloc(total_length);
    if (!result) {
        perror("Failed to allocate memory for password_requirement");
        return;
    }

    // Format the string as space-separated integers
    snprintf(
        result,
        total_length,
        "%d %d %d %d",
        requirement->length,
        requirement->uppercased,
        requirement->digits,
        requirement->special_characters);

    fputs(result, file);
    fputc('\n', file);
    free(result);
}


void save_passwords(struct password** passwords, const int* curr_size, FILE* file) {
    struct password** ptr = passwords;
    for (int i = 0; i < *curr_size; i++, ptr++) {
        if ((*ptr) != NULL) {
            // Calculate the required buffer size (2 for space, 1 for '\0')
            const size_t length = strlen((*ptr)->name) + strlen((*ptr)->username) + strlen((*ptr)->password) + 3;
            char *result = malloc(length);
            if (!result)
                continue;
            snprintf(result, length, "%s %s %s", (*ptr)->name, (*ptr)->username, (*ptr)->password);
            fputs(result, file);
            fputc('\n', file);
            free(result);
            free((*ptr)->name);
            free((*ptr)->username);
            free((*ptr)->password);
            free((*ptr));
        }
    }
}


void save_passwords_and_requirements(
    struct password_requirement *requirements,
    struct password **passwords,
    const int* curr_size,
    const char *file_name) {
    FILE* file = fopen(file_name, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }
    save_password_requirement(requirements, file);
    save_passwords(passwords, curr_size, file);
    fclose(file);
}
