#ifndef SMART_WALLET_H
#define SMART_WALLET_H

#include "basic.h"

// Local Data Methods
void wallet_get(struct wallet_st *, const struct string_st *, const struct string_st *);

// NetWork Methods
void wallet_request(struct wallet_st *, const struct string_st *, const struct string_st *);
void wallet_response(struct string_st *, const struct string_st *);

#endif //SMART_WALLET_H
