#ifndef SMART_GENERATION_H
#define SMART_GENERATION_H

#include "basic.h"

// Local Files Methods
void file_write(const struct string_st *, const struct string_st *);
void file_read(const struct string_st *, struct string_st *);
int file_exist(const struct string_st *);
int file_remove(const struct string_st *);

// Local Data Methods
void history_generation_get(struct list_st *, const struct integer_st *, const struct integer_st *);
void generation_get(struct generation *, const struct string_st *, const struct integer_st *);
void generation_save_local(const struct generation *);
int generation_exist(const struct string_st *, const struct integer_st *);
void generation_load(struct generation *, const struct string_st *, const struct integer_st *);

// NetWork Methods
void generation_request(struct generation *, const struct string_st *, const struct integer_st *);
void generation_response(struct string_st *, const struct string_st *);

void history_generation_request(struct list_st *, const struct integer_st *, const struct integer_st *);
void history_generation_response(struct string_st *, const struct string_st *);

// Class Methods
void generate_make_hash(struct generation *);
int generate_check_hash(const struct generation *);
int generation_work(const struct generation *);

int cre_generation_wallet_create(const struct string_st *, const struct string_st *);
int cre_generation_account_username(const struct string_st *, const struct account_st *, const struct string_st *);
int cre_generation_block_create(const struct block_st *);
int cre_generation_block_result(const struct block_st *, int, const struct account_st *, const struct string_st *);

#endif //SMART_GENERATION_H
