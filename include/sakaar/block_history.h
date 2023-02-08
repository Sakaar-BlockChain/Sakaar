#ifndef SMART_BLOCK_HISTORY_H
#define SMART_BLOCK_HISTORY_H

#include "basic.h"

// Local Data Methods
void history_get(struct list_st *, const struct integer_st *, const struct integer_st *);
void block_history_get(struct block_history *, const struct string_st *, const struct integer_st *);
void block_history_save_local(const struct block_history *);
int block_history_exist(const struct string_st *, const struct integer_st *);
void block_history_load(struct block_history *, const struct string_st *, const struct integer_st *);

// NetWork Methods
void block_history_request(struct block_history *, const struct string_st *, const struct integer_st *);
void block_history_response(struct string_st *, const struct string_st *);

void history_request(struct list_st *, const struct integer_st *, const struct integer_st *);
void history_response(struct string_st *, const struct string_st *);

#endif //SMART_BLOCK_HISTORY_H
