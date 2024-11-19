//
// Created by Nicola Hermann on 18.11.2024.
//

#ifndef VAULT_MENU_H
#define VAULT_MENU_H

#include "password.h"

void generate_and_save_password(
    struct password **p_passwords,
    int *p_num_passwords,
    const struct password_requirement *requirements);
void add_existing_password(struct password **p_passwords, int *p_num_passwords, const struct password_requirement *requirements);
void get_password(struct password **p_passwords, const int num_passwords);
void list_password_names(struct password** passwords, const int *num_passwords);
void edit_password(struct password** passwords, int num_passwords, const struct password_requirement *requirements);
void loop_delete_password(struct password*** passwords, int* num_passwords);
void update_password_requirements(struct password_requirement* req);

#endif //VAULT_MENU_H
