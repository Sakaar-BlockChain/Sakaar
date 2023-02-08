#ifndef SMART_WALLET_DATA_H
#define SMART_WALLET_DATA_H

#include "basic.h"

// Local Data Methods
void wallet_data_get(struct wallet_data *, const struct string_st *, const struct string_st *);
void wallet_data_save_local(const struct wallet_data *);
int wallet_data_exist(const struct string_st *, const struct string_st *);
void wallet_data_load(struct wallet_data *, const struct string_st *, const struct string_st *);

// NetWork Methods
void wallet_data_request(struct wallet_data *, const struct string_st *, const struct string_st *);
void wallet_data_response(struct string_st *, const struct string_st *);

#endif //SMART_WALLET_DATA_H
