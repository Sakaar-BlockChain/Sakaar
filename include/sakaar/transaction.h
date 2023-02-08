#ifndef SMART_TRANSACTION_H
#define SMART_TRANSACTION_H

#include "basic.h"

// Class Methods
void transaction_set_data(struct transaction_st *, struct pre_transaction *, struct string_st *, struct integer_st *);
void transaction_get_hash(const struct transaction_st *, struct string_st *, const struct integer_st *);
void transaction_create_sign(const struct transaction_st *, const struct string_st *, const struct currency_st *, struct string_st *);

#endif //SMART_TRANSACTION_H
