#ifndef SMART_TRANSACTION_H
#define SMART_TRANSACTION_H

#include "struct.h"
#include "pre_transaction.h"

struct transaction_st {
    struct string_st *address_from;
    struct string_st *address_to;
    struct string_st *currency;

    struct integer_st *balance;
    struct integer_st *fee;

    struct integer_st *balance_from;
    struct string_st *hash_from;

    struct string_st *signature;
};


struct transaction_st *transaction_new();
void transaction_set(struct transaction_st *, const struct transaction_st *a);
void transaction_clear(struct transaction_st *);
void transaction_free(struct transaction_st *);

void transaction_set_data(struct transaction_st *, struct pre_transaction *, struct string_st *, struct integer_st *);
void transaction_get_hash(const struct transaction_st *, struct string_st *, const struct integer_st *);
void transaction_create_sign(const struct transaction_st *, const struct string_st *, const struct currency_st *, struct string_st *);

void transaction_set_tlv(struct transaction_st *, const struct string_st *);
void transaction_get_tlv(const struct transaction_st *, struct string_st *);

#endif //SMART_TRANSACTION_H
