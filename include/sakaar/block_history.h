#ifndef SMART_BLOCK_HISTORY_H
#define SMART_BLOCK_HISTORY_H

#include "struct.h"

struct block_history {
    struct list_st *transactions;
    struct integer_st *benefit;
    struct integer_st *time;
    struct string_st *address_outside;
    struct string_st *hash;
    struct string_st *smart_contract;
    struct integer_st *result;
};


struct block_history *block_history_new();
void block_history_set(struct block_history *, const struct block_history *);
void block_history_clear(struct block_history *);
void block_history_free(struct block_history *);

void history_get(struct list_st *, const struct integer_st *, const struct integer_st *);
void block_history_get(struct block_history *, const struct string_st *, const struct integer_st *);
void block_history_set_tlv(struct block_history *, const struct string_st *);
void block_history_get_tlv(const struct block_history *, struct string_st *);


void block_history_save_local(const struct block_history *);
int block_history_exist(const struct string_st *, const struct integer_st *);
void block_history_load(struct block_history *, const struct string_st *, const struct integer_st *);

void block_history_request(struct block_history *, const struct string_st *, const struct integer_st *);
void block_history_response(struct string_st *, const struct string_st *);

void history_request(struct list_st *, const struct integer_st *, const struct integer_st *);
void history_response(struct string_st *, const struct string_st *);

#endif //SMART_BLOCK_HISTORY_H
