#ifndef SMART_ACCOUNT_CONNECTIONS_H
#define SMART_ACCOUNT_CONNECTIONS_H

#include "basic.h"

// Local Data Methods
void account_connections_get(struct account_connections *, const struct string_st *, const struct string_st *);
void account_connections_save_local(const struct account_connections *);
void account_connections_load(struct account_connections *, const struct string_st *, const struct string_st *);

// NetWork Methods
void account_connections_request(struct account_connections *, const struct string_st *, const struct string_st *);
void account_connections_response(struct string_st *, const struct string_st *);

// Class Methods
void account_connections_add(struct account_connections *, const struct string_st *);
void account_connections_remove(struct account_connections *, const struct string_st *);
int account_connections_contains(const struct account_connections *, const struct string_st *);

#endif //SMART_ACCOUNT_CONNECTIONS_H
