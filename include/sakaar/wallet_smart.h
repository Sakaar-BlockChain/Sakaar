#ifndef SMART_WALLET_SMART_H
#define SMART_WALLET_SMART_H

#include "basic.h"

// Local Data Methods
void wallet_smart_get(struct wallet_smart *, const struct string_st *, const struct string_st *);
void wallet_smart_save_local(const struct wallet_smart *);
void wallet_smart_load(struct wallet_smart *, const struct string_st *, const struct string_st *);

// NetWork Methods
void wallet_smart_request(struct wallet_smart *, const struct string_st *, const struct string_st *);
void wallet_smart_response(struct string_st *, const struct string_st *);

// Class Methods
void wallet_smart_private(const struct wallet_smart *, const struct string_st *, const struct account_st *, struct string_st *);
void wallet_smart_get_contract(const struct wallet_smart *, const struct currency_st *, struct string_st *);

#endif //SMART_WALLET_SMART_H
