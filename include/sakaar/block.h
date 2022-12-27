#ifndef SMART_BLOCK_H
#define SMART_BLOCK_H

#include "struct.h"
#include "time.h"
#include "block_history.h"

struct block_st {
    struct list_st *transactions;
    struct integer_st *benefit;
    struct integer_st *time;
    struct string_st *address_outside;
    struct string_st *hash;
    struct string_st *smart_contract;
    struct list_st *nodes_done;
    struct integer_st *result_pros;
    struct integer_st *result_cons;
    struct integer_st *voted;
};


struct block_st *block_new();
void block_set(struct block_st *, const struct block_st *);
void block_clear(struct block_st *);
void block_free(struct block_st *);
int block_is_null(const struct block_st *);

void make_block(struct block_st *, const struct list_st *, const struct string_st *, const struct string_st *);
void block_get_history(const struct block_st *, int, struct block_history *);
void block_get_optimal_unchecked(struct block_st *);
void block_get_optimal_checked(struct block_st *, int *);

void block_get(struct block_st *, const struct string_st *, const struct integer_st *);
void block_set_tlv(struct block_st *, const struct string_st *);
void block_get_tlv(const struct block_st *, struct string_st *);


void block_save_local(const struct block_st *);
int block_exist(const struct string_st *, const struct integer_st *);
void block_load(struct block_st *, const struct string_st *, const struct integer_st *);
int block_remove(const struct string_st *, const struct integer_st *);

void block_request(struct block_st *, const struct string_st *, const struct integer_st *);
void block_response(struct string_st *, const struct string_st *);

#endif //SMART_BLOCK_H
