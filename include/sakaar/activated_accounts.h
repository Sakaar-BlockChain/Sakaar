#ifndef SMART_ACTIVATED_ACCOUNTS_H
#define SMART_ACTIVATED_ACCOUNTS_H

#include "basic.h"

// Local Data Methods
void activated_accounts_add(struct activated_accounts *, const struct string_st *, const struct integer_st *);
void activated_accounts_remove(struct activated_accounts *, const struct string_st *);
int activated_accounts_contains(const struct activated_accounts *, const struct string_st *);
void activated_accounts_freeze(const struct activated_accounts *, struct integer_st *);

// NetWork Methods
void activated_accounts_get(struct activated_accounts *);
void activated_accounts_save_local(const struct activated_accounts *);
void activated_accounts_load(struct activated_accounts *);

// Class Methods
void activated_accounts_request(struct activated_accounts *);
void activated_accounts_response(struct string_st *, const struct string_st *);

#endif //SMART_ACTIVATED_ACCOUNTS_H
