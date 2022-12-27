#ifndef SMART_ACCOUNT_CONNECTIONS_H
#define SMART_ACCOUNT_CONNECTIONS_H

#include "struct.h"

struct account_connections {
    struct string_st *address;
    struct string_st *currency;
    struct list_st *addresses;
};


struct account_connections *account_connections_new();
void account_connections_set(struct account_connections *, const struct account_connections *);
void account_connections_clear(struct account_connections *);
void account_connections_free(struct account_connections *);

void account_connections_add(struct account_connections *, const struct string_st *);
void account_connections_remove(struct account_connections *, const struct string_st *);
int account_connections_contains(const struct account_connections *, const struct string_st *);

void account_connections_get(struct account_connections *, const struct string_st *, const struct string_st *);
void account_connections_set_tlv(struct account_connections *, const struct string_st *);
void account_connections_get_tlv(const struct account_connections *, struct string_st *);


void account_connections_save_local(const struct account_connections *);
void account_connections_load(struct account_connections *, const struct string_st *, const struct string_st *);

void account_connections_request(struct account_connections *, const struct string_st *, const struct string_st *);
void account_connections_response(struct string_st *, const struct string_st *);

#endif //SMART_ACCOUNT_CONNECTIONS_H
