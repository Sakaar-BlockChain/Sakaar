#ifndef SMART_CURRENCY_H
#define SMART_CURRENCY_H

#include "basic.h"

// Local Data Methods
void currency_get(struct currency_st *, const struct string_st *);
void currency_save_local(const struct currency_st *);
int currency_exist(const struct string_st *);
void currency_load(struct currency_st *, const struct string_st *);

// NetWork Methods
void currency_request(struct currency_st *, const struct string_st *);
void currency_response(struct string_st *, const struct string_st *);

// Class Methods
void balance_outside(const struct currency_st *, const struct string_st *, struct integer_st *);
void register_outside(const struct currency_st *, const struct string_st *, struct string_st *);
void send_outside(const struct currency_st *, const struct string_st *, const struct string_st *, const struct integer_st *);

#endif //SMART_CURRENCY_H
