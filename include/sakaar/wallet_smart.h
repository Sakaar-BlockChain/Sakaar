#ifndef SMART_WALLET_SMART_H
#define SMART_WALLET_SMART_H

#include "struct.h"
#include "currency.h"

struct wallet_smart {
    struct string_st *address;
    struct string_st *currency;
    struct string_st *private_key;
    struct string_st *smart_private;
    struct string_st *smart_contract;
    struct string_st *name;
    struct string_st *owner;

    struct integer_st *freeze;
};


struct wallet_smart *wallet_smart_new();
void wallet_smart_set(struct wallet_smart *, const struct wallet_smart *);
void wallet_smart_clear(struct wallet_smart *);
void wallet_smart_free(struct wallet_smart *);

void wallet_smart_private(const struct wallet_smart *, const struct string_st *, const struct account_st *, struct string_st *);
void wallet_smart_get_contract(const struct wallet_smart *, const struct currency_st *, struct string_st *);

void wallet_smart_get(struct wallet_smart *, const struct string_st *, const struct string_st *);
void wallet_smart_set_tlv(struct wallet_smart *, const struct string_st *);
void wallet_smart_get_tlv(const struct wallet_smart *, struct string_st *);


void wallet_smart_save_local(const struct wallet_smart *);
void wallet_smart_load(struct wallet_smart *, const struct string_st *, const struct string_st *);

void wallet_smart_request(struct wallet_smart *, const struct string_st *, const struct string_st *);
void wallet_smart_response(struct string_st *, const struct string_st *);


#endif //SMART_WALLET_SMART_H
