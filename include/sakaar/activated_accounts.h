#ifndef SMART_ACTIVATED_ACCOUNTS_H
#define SMART_ACTIVATED_ACCOUNTS_H

#include "struct.h"

struct activated_accounts {
    struct list_st *addresses;
};


struct activated_accounts *activated_accounts_new();
void activated_accounts_set(struct activated_accounts *, const struct activated_accounts *);
void activated_accounts_clear(struct activated_accounts *);
void activated_accounts_free(struct activated_accounts *);

void activated_accounts_add(struct activated_accounts *, const struct string_st *, const struct integer_st *);
void activated_accounts_remove(struct activated_accounts *, const struct string_st *);
int activated_accounts_contains(const struct activated_accounts *, const struct string_st *);
void activated_accounts_freeze(const struct activated_accounts *, struct integer_st *);

void activated_accounts_get(struct activated_accounts *);
void activated_accounts_set_tlv(struct activated_accounts *, const struct string_st *);
void activated_accounts_get_tlv(const struct activated_accounts *, struct string_st *);


void activated_accounts_save_local(const struct activated_accounts *);
void activated_accounts_load(struct activated_accounts *);

void activated_accounts_request(struct activated_accounts *);
void activated_accounts_response(struct string_st *, const struct string_st *);

#endif //SMART_ACTIVATED_ACCOUNTS_H
