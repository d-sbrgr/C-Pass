//
// Created by dave_ on 27.10.2024.
//

#include "password.h"

#include <stdio.h>

#include "util.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

#define DEFAULT_LENGTH 20


const char *digits = "0123456789";
const char *special_characters = "!@#$%^&*()-_=+[]{}|;:,.<>?/~";
const char *alpha_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *alpha_lower = "abcdefghijklmnopqrstuvwxyz";


/*
 * Read in a password from console without displaying the plain characters
 *
 * return char*: The password entered by the user
 */
char* read_password() {
    char *password = calloc(DEFAULT_LENGTH, sizeof(char));
    int i = 0;
    char ch;
    printf("Enter password: ");

#if defined(_WIN32)
    while (true) {
        if (i % DEFAULT_LENGTH == 0 && i > 0) {
            char* temp = realloc(password, i + DEFAULT_LENGTH);
            if (temp == NULL) {
                free(password);
                return NULL;
            }
            free(password);
            password = temp;
        }
        ch = _getch();
        if (ch == '\r' || ch == '\n')
            break;
        if (ch == '\b' && i > 0) {
            // Handle backspace
            printf("\b \b");
            i--;
        } else {
            password[i++] = ch;
            printf("*"); // Mask character
        }
    }
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while ((ch = getchar()) != '\n') {
        if (i % DEFAULT_LENGTH == 0 && i > 0) {
            char* temp = realloc(password, i + DEFAULT_LENGTH);
            if (temp == NULL) {
                free(password);
                return NULL;
            }
            free(password);
            password = temp;
        }
        if (ch == 127 || ch == '\b' && i > 0) {
            // Handle backspace
            i--;
            printf("\b \b");
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    password[i] = '\0'; // Null-terminate the password
    printf("\n");
    return password;
}

/*
 * Generate a random new password that matches the requirements
 *
 * param struct password_requirement* requirement: Pointer to the minimum password requirement defined by the user
 * return char*: Newly created character array with the new password
 */
char *generate_password(const struct password_requirement *requirement) {
    if (requirement->length < requirement->uppercased + requirement->digits + requirement->special_characters) {
        return NULL;
    }
    // Seed the random number generator
    char *password = calloc(requirement->length, sizeof(char));

    srand(time(NULL));

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


/*
 * Deletes the password in the password array at the given index.
 * Frees all memory and sets the array at the given index to NULL
 *
 * param struct password*** arr: Pointer to the array containing the password struct pointers
 * param const int* index: Pointer to the integer depicting the given index
 */
void delete_password(struct password ***arr, const int index) {
    if (*arr == NULL)
        return;
    struct password *temp = (*arr)[index];
    free(temp->name);
    free(temp->username);
    free(temp->password);
    free(temp);
    (*arr)[index] = NULL;
}


/*
 * Changes the password in the password array at the given index.
 * Frees the memory of the old password string and allocates new memory in the password struct,
 * where the new password is stored
 *
 * param struct password*** arr: Pointer to the array containing the password struct pointers
 * param const int* index: Pointer to the integer depicting the given index
 * param const char* password: Character array containing the string of the new password
 */
void change_password(struct password ***arr, const int *index, const char *password) {
    if (*arr == NULL)
        return;
    struct password *temp = (*arr)[*index];
    free(temp->password);
    temp->password = strdup(password);
}


/*
 * Add a new password to the array containing the password structs.
 * Dynamically allocates new memory if the array is currently full and initializes newly
 * allocated memory with NULL
 *
 * param struct password*** arr: Pointer to the array containing the password struct pointers
 * param int* curr_size: Pointer to the integer describing the current size of the array
 * param const char* name: String containing the name of the new entry
 * param const char* username: String containing the username of the new entry
 * param const char* password: String containing the password of the new entry
 */
void add_password(
    struct password ***arr,
    int *curr_size,
    const char *name,
    const char *username,
    const char *password) {
    if (*curr_size % DEFAULT_CAPACITY == 0 && *curr_size > 0) {
        struct password **new_array = realloc(*arr, *curr_size + DEFAULT_CAPACITY * sizeof(struct password *));
        if (!new_array) {
            perror("Failed to resize array");
            return;
        }
        *arr = new_array;
        struct password **ptr = (*arr) + *curr_size;
        // Initialize newly allocated pointers to NULL
        for (size_t i = *curr_size; i < *curr_size + DEFAULT_CAPACITY; i++, ptr++) {
            *ptr = NULL;
        }
    }

    struct password **ptr = (*arr) + *curr_size;

    // Allocate memory for the new password entry
    *ptr = calloc(1, sizeof(struct password));
    (*ptr)->name = strdup(name);
    (*ptr)->username = strdup(username);
    (*ptr)->password = strdup(password);
    (*curr_size)++;
}


/*
 * Reads all stored passwords in the given file.
 * If the file exists, skip the first line, because this is holding the password requirements.
 * From the second line on, read every line and initialize the password structs accordingly.
 *
 * param const char* input: Character array containing the cleartext file contents
 * param int* curr_size: Pointer to the integer where the current size of the array is stored
 * return struct password**: Array of pointers to password structs
 */
struct password **read_passwords(const char *input, int *curr_size) {
    struct password **p_passwords = calloc(DEFAULT_CAPACITY, sizeof(struct password *));
    *curr_size = 0;
    // If there are no passwords yet, return empty array
    if (!input || *input == '\0') {
        return p_passwords;
    }
    char *temp = strdup(input);

    char *line = temp;
    char *next_line;
    next_line = strchr(line, '\n');
    line = next_line + 1;

    // Loop through each line
    while ((next_line = strchr(line, '\n')) != NULL) {
        *next_line = '\0';
        char *words = strdup(line);
        const size_t len = strlen(words);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        const char *name = strtok(words, " ");
        const char *username = strtok(NULL, " ");
        const char *password = strtok(NULL, " ");
        add_password(&p_passwords, curr_size, name, username, password);
        line = next_line + 1; // Move to the next line
        free(words);
    }

    free(temp);
    return p_passwords;
}


/*
 * Read the password requirement saved in the password file.
 * If none has been defined yet, a default is returned.
 * Default = (length: 12, uppercased letters: 1, digits: 1, special characters: 1)
 *
 * param const char* input: Character array containing the cleartext file contents
 * return struct password_requirement*: Pointer to the struct containing the defined password requirements
*/
struct password_requirement *read_password_requirement(const char *input) {
    struct password_requirement *p_requirement = calloc(1, sizeof(struct password_requirement));
    p_requirement->length = 12;
    p_requirement->uppercased = 1;
    p_requirement->digits = 1;
    p_requirement->special_characters = 1;

    // If there is no requirement set, return default requirement
    if (!input) {
        return p_requirement;
    }
    char *temp = strdup(input);

    // Parse the line for integers separated by spaces
    char *line = strtok(temp, "\n");
    const char *token = strtok(line, " ");
    if (token) p_requirement->length = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->uppercased = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->digits = atoi(token);
    token = strtok(NULL, " ");
    if (token) p_requirement->special_characters = atoi(token);
    free(temp);
    return p_requirement;
}


/*
 * Saves the given password requirement to the open file stream
 *
 * param struct password_requirement* requirement: Pointer to the current password requirement
 * return char*: Character array displaying the password requirement as string
 */
char * get_password_requirement(struct password_requirement *requirement) {
    char buffer[50];
    const int len_length = snprintf(buffer, sizeof(buffer), "%d", requirement->length);
    const int len_digits = snprintf(buffer, sizeof(buffer), "%d", requirement->digits);
    const int len_uppercased = snprintf(buffer, sizeof(buffer), "%d", requirement->uppercased);
    const int len_special = snprintf(buffer, sizeof(buffer), "%d", requirement->special_characters);

    // Total length: sum of lengths + 3 spaces + 1 for '\0'
    const size_t size = len_length + len_digits + len_uppercased + len_special + 5;

    char * output = calloc(size, sizeof(char));
    if (!output) {
        perror("Failed to allocate memory for password_requirement");
        return NULL;
    }

    // Format the string as space-separated integers
    snprintf(
        output,
        size,
        "%d %d %d %d\n",
        requirement->length,
        requirement->uppercased,
        requirement->digits,
        requirement->special_characters);
    return output;
}


/*
 * Saves the passwords in the array to the open file stream
 * After saving each password, the memory used in the password struct pointer is freed
 *
 * param struct passwords** passwords: Array of password struct pointers
 * param const int* curr_size: Current size of the password array
 * return char*: Character array displaying the passwords as string seperated by newline
 */
char * get_passwords(struct password **passwords, const int *curr_size) {
    size_t size = 1024;
    char *output= calloc(size, sizeof(char));
    if (!output) {
        perror("Failed to allocate memory for password_requirement");
        return NULL;
    }
    output[0] = '\0';
    struct password **ptr = passwords;
    for (int i = 0; i < *curr_size; i++, ptr++) {
        if (*ptr != NULL) {
            // Calculate the required buffer size (2 for space, 1 for '\0')
            const size_t length = strlen((*ptr)->name) + strlen((*ptr)->username) + strlen((*ptr)->password) + 4;
            char *pw = malloc(length);
            if (!pw) {
                free((*ptr)->name);
                free((*ptr)->username);
                free((*ptr)->password);
                free(*ptr);
                continue;
            }
            snprintf(pw, length, "%s %s %s\n", (*ptr)->name, (*ptr)->username, (*ptr)->password);

            free((*ptr)->name);
            free((*ptr)->username);
            free((*ptr)->password);
            free(*ptr);
            if (strlen(pw) + strlen(output) + 1 > size) {
                size *= 2;
                char * temp = realloc(output, size);
                if (!temp) {
                    perror("Failed to resize array");
                    free(pw);
                    free(output);
                    return NULL;
                }
                output = temp;
                free(temp);
            }
            strcat(output, pw);
            free(pw);
        }
    }
    return output;
}


/*
 * Saves the password requirement and the passwords to a given file.
 * Frees all memory used in the password struct pointers
 *
 * param struct password_requirement* requirements: Pointer to the current password requirement struct
 * param struct password** passwords: Array of password struct pointers
 * param const int* curr_size: Pointer to the integer holding the current array size
 */
void save_passwords_and_requirements(
    struct password_requirement *requirements,
    struct password **passwords,
    const int *curr_size,
    char **output) {
    char *req = get_password_requirement(requirements);
    char *pwd = get_passwords(passwords, curr_size);
    if (!req || !pwd) {
        perror("Failed to allocate memory for password_requirement");
        free(req);
        free(pwd);
        return;
    }
    *output = calloc(strlen(req) + strlen(pwd) + 1, sizeof(char));
    strcat(*output, req);
    strcat(*output, pwd);
    free(req);
    free(pwd);
}

/*
 * Check whether a given password meets the requirements
 *
 * param const char* password: Character array containing the password in question
 * param const struct* password_requirement: Point to given password_requirement struct
 * return int: 1 if password is valid, else 0
 */
int is_valid_password(const char *password, const struct password_requirement *requirement) {
    const int length = strlen(password);
    int num_uppercased = 0, num_digits = 0, num_special_characters = 0;

    if (length < requirement->length) {
        return 0;  // Password is too short
    }

    // Count the required character types in the password
    for (int i = 0; i < length; i++) {
        if (isupper(password[i])) {
            num_uppercased++;
        } else if (isdigit(password[i])) {
            num_digits++;
        } else if (strchr(special_characters, password[i]) != NULL) {
            num_special_characters++;
        }
    }

    // Check if the password meets the requirements
    if (num_uppercased >= requirement->uppercased && num_digits >= requirement->digits && num_special_characters >= requirement->special_characters) {
        return 1;  // Password is valid
    }

    return 0;  // Password is invalid
}
