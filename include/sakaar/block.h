#ifndef SMART_BLOCK_H
#define SMART_BLOCK_H

#include "basic.h"
#include "time.h"

// Local Data Methods
void block_get(struct block_st *, const struct string_st *, const struct integer_st *);
void block_save_local(const struct block_st *);
int block_exist(const struct string_st *, const struct integer_st *);
void block_load(struct block_st *, const struct string_st *, const struct integer_st *);
int block_remove(const struct string_st *, const struct integer_st *);

// NetWork Methods
void block_request(struct block_st *, const struct string_st *, const struct integer_st *);
void block_response(struct string_st *, const struct string_st *);

// Class Methods
void make_block(struct block_st *, const struct list_st *, const struct string_st *, const struct string_st *);
void block_get_history(const struct block_st *, int, struct block_history *);
void block_get_optimal_unchecked(struct block_st *);
void block_get_optimal_checked(struct block_st *, int *);

#endif //SMART_BLOCK_H
