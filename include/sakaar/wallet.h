#ifndef SMART_WALLET_H
#define SMART_WALLET_H

#include "struct.h"
#include "wallet_data.h"
#include "wallet_smart.h"

struct wallet_st {
    struct wallet_data *data;
    struct wallet_smart *smart;
};


struct wallet_st *wallet_new();
void wallet_set(struct wallet_st *, const struct wallet_st *);
void wallet_clear(struct wallet_st *);
void wallet_free(struct wallet_st *);

void wallet_get(struct wallet_st *, const struct string_st *, const struct string_st *);
void wallet_set_tlv(struct wallet_st *, const struct string_st *);
void wallet_get_tlv(const struct wallet_st *, struct string_st *);


void wallet_request(struct wallet_st *, const struct string_st *, const struct string_st *);
void wallet_response(struct string_st *, const struct string_st *);


#endif //SMART_WALLET_H
