#ifndef SMART_CONFIG_H
#define SMART_CONFIG_H

#include "basic.h"
#include "time.h"
#include "network.h"

struct config_st {
    struct account_st *account;
    struct string_st *key;

    struct network_p2p *network;
    struct integer_st *full_freeze;
    int _is_server;
};
extern struct config_st *main_config;
// Standard operations
struct config_st *config_new();
void config_clear(struct config_st *);
void config_free(struct config_st *);

// TLV Methods
void config_set_tlv(struct config_st *res, const struct string_st *tlv);
void config_get_tlv(const struct config_st *config, struct string_st *res);

// Local Data Methods
void config_save_local(const struct config_st *);
void config_load(struct config_st *);

// NetWork Methods
void config_server_start();
void config_server_close();

void config_network_get(const struct string_st *, struct string_st *);
void config_network_send(const struct string_st *);

// Class Methods
void config_account_log_in(const struct string_st *, const struct string_st *);
void config_account_register(const struct string_st *, const struct string_st *);
void config_account_activate();
void config_account_deactivate();
void config_account_username(const struct string_st *);
void config_account_freeze(const struct string_st *, const struct integer_st *);

void config_currency_create(const struct string_st *, const struct string_st *, const struct string_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *, const struct integer_st *);

void config_wallet_create(const struct string_st *, const struct string_st *, const struct string_st *);
void config_wallet_delete(const struct string_st *, const struct string_st *);

void config_block_create(const struct block_st *);

void config_make_pre_transaction(struct pre_transaction *, struct string_st *, struct string_st *, struct string_st *, struct integer_st *, struct integer_st *);


#endif //SMART_CONFIG_H
