//
// Created by dave_ on 27.10.2024.
//

#ifndef PASSWORD_H
#define PASSWORD_H

#define DEFAULT_CAPACITY 32

struct password {
    char* name;
    char* username;
    char* password;
};

struct password_requirement {
    int length;
    int uppercased;
    int digits;
    int special_characters;
};

char* read_password();
char* generate_password(const struct password_requirement* requirement);
void add_password(
    struct password ***arr,
    int *curr_size,
    const char *name,
    const char* username,
    const char *password);
void delete_password(struct password*** arr, const int* index);
void change_password(struct password*** arr, const int* index, const char* password);
struct password** read_passwords(const char* file_name, int* curr_size);
struct password_requirement* read_password_requirement(const char* file_name);
void save_passwords_and_requirements(
    struct password_requirement* requirements,
    struct password** passwords,
    const int* curr_size,
    char** output);

int is_valid_password(const char *password, const struct password_requirement *requirement);

extern const char *digits;
extern const char *special_characters;
extern const char *alpha_upper;
extern const char *alpha_lower;

#endif //PASSWORD_H
