#ifndef SMART_ACCOUNT_H
#define SMART_ACCOUNT_H

#include "basic.h"

// Local Data Methods
void account_get(struct account_st *, const struct string_st *);
void account_save_local(const struct account_st *);
int account_exist(const struct string_st *);
void account_load(struct account_st *, const struct string_st *);

// NetWork Methods
void account_request(struct account_st *, const struct string_st *);
void account_response(struct string_st *, const struct string_st *);

#endif //SMART_ACCOUNT_H
