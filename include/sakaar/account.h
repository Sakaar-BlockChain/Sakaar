#ifndef SMART_ACCOUNT_H
#define SMART_ACCOUNT_H

#include "struct.h"

struct account_st {
    struct string_st *address;
    struct string_st *login;

    struct integer_st *activated;
    struct integer_st *freeze;

    struct integer_st *hash_type;
    struct integer_st *crypto_type;
    struct integer_st *crypto_base;
};


struct account_st *account_new();
void account_set(struct account_st *, const struct account_st *);
void account_clear(struct account_st *);
void account_free(struct account_st *);
int account_is_null(const struct account_st *);

void account_get(struct account_st *, const struct string_st *);
void account_set_tlv(struct account_st *, const struct string_st *);
void account_get_tlv(const struct account_st *, struct string_st *);


void account_save_local(const struct account_st *);
int account_exist(const struct string_st *);
void account_load(struct account_st *, const struct string_st *);

void account_request(struct account_st *, const struct string_st *);
void account_response(struct string_st *, const struct string_st *);

#endif //SMART_ACCOUNT_H
