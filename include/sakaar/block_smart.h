#ifndef SMART_BLOCK_SMART_H
#define SMART_BLOCK_SMART_H

#include "struct.h"

void cre_block_smart_currency_create(const struct string_st *, const struct account_st *, const struct string_st *, const struct string_st *, const struct string_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *);

void cre_block_smart_wallet_connect(const struct string_st *, const struct string_st *, const struct string_st *, const struct string_st *, const struct account_st *, const struct string_st *);
void cre_block_smart_wallet_disconnect(const struct string_st *, const struct string_st *, const struct account_st *, const struct string_st *);

void cre_block_smart_account_create(const struct string_st *, int, int, int, const struct string_st *);
void cre_block_smart_account_freeze(const struct string_st *, const struct integer_st *, const struct account_st *, const struct string_st *);
void cre_block_smart_account_activate(const struct account_st *, const struct string_st *);
void cre_block_smart_account_deactivate(const struct account_st *, const struct string_st *);


int block_smart_check(const struct string_st *res);
void block_smart_make(const struct string_st *res);

#endif //SMART_BLOCK_SMART_H
