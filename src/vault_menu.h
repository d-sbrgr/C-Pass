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
#endif //VAULT_MENU_H
