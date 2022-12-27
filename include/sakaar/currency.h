#ifndef SMART_CURRENCY_H
#define SMART_CURRENCY_H

#include "struct.h"

struct currency_st {
    struct string_st *name;
    struct string_st *smart_contract;
    struct string_st *info;

    struct integer_st *hash_type;
    struct integer_st *crypto_type;
    struct integer_st *crypto_base;

    struct integer_st *our;
    struct integer_st *product;
};


struct currency_st *currency_new();
void currency_set(struct currency_st *, const struct currency_st *);
void currency_clear(struct currency_st *);
void currency_free(struct currency_st *);

void balance_outside(const struct currency_st *, const struct string_st *, struct integer_st *);
void register_outside(const struct currency_st *, const struct string_st *, struct string_st *);
void send_outside(const struct currency_st *, const struct string_st *, const struct string_st *, const struct integer_st *);

void currency_get(struct currency_st *, const struct string_st *);
void currency_set_tlv(struct currency_st *, const struct string_st *);
void currency_get_tlv(const struct currency_st *, struct string_st *);


void currency_save_local(const struct currency_st *);
int currency_exist(const struct string_st *);
void currency_load(struct currency_st *, const struct string_st *);

void currency_request(struct currency_st *, const struct string_st *);
void currency_response(struct string_st *, const struct string_st *);


#endif //SMART_CURRENCY_H
