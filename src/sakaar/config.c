#include "sakaar.h"
#include "crypto_base.h"

struct config_st *main_config = NULL;


void blocks_restore();
int block_check(const struct block_st *block);
void block_save(const struct block_st *block, int result);

void *block_checker(void *arg) {
    struct block_st *block = block_new();
    int result;
    while (main_config->_is_server) {
        if (!integer_is_null(main_config->account->activated)) {
            block_get_optimal_unchecked(block);
            if (!block_is_null(block)) {
                result = block_check(block);
                cre_generation_block_result(block, result, main_config->account, main_config->key);
            }
        }
        {
            block_get_optimal_checked(block, &result);
            if (!block_is_null(block)) {
                if (result) {
                    block_save(block, 1);
                } else {
                    block_save(block, 0);
                }
            }
        }
    }
    block_free(block);
    return NULL;
}


void config_request_get(const struct string_st *data, struct string_st *response) {
    if (response == NULL) return;
    if (string_is_null(data)) return string_clear(response);

    switch (tlv_get_tag(data->data)) {
        case TLV_REQ_ACCOUNT:
            return account_response(response, data);
        case TLV_REQ_ACCOUNT_CONN:
            return account_connections_response(response, data);
        case TLV_REQ_ACTIVE_ACC:
            return activated_accounts_response(response, data);
        case TLV_REQ_BLOCK:
            return block_response(response, data);
        case TLV_REQ_BLOCK_HISTORY:
            return block_history_response(response, data);
        case TLV_REQ_CURRENCY:
            return currency_response(response, data);
        case TLV_REQ_WALLET:
            return wallet_response(response, data);
        case TLV_REQ_WALLET_DATA:
            return wallet_data_response(response, data);
        case TLV_REQ_WALLET_SMART:
            return wallet_smart_response(response, data);
        case TLV_REQ_BLOCK_HISTORY_LIST:
            return history_response(response, data);
        case TLV_REQ_GENERATION:
            return generation_response(response, data);
        case TLV_REQ_GENERATION_LIST:
            return history_generation_response(response, data);
            // TODO make change of coins
    }
}
int config_request_send(const struct string_st *data) {
    if (string_is_null(data)) return 0;
    if (tlv_get_tag(data->data) != TLV_GENERATION) return 0;

    struct generation *gen = generation_new();
    generation_set_tlv(gen, data);
    int _result = generation_work(gen);
    generation_free(gen);
    return _result;
}

// Standard operations
struct config_st *config_new() {
    struct config_st *res = skr_malloc(sizeof(struct config_st));

    struct network_conf config = (struct network_conf) {AF_INET, SOCK_STREAM, 0, INADDR_ANY, 1240, 20};
    res->account = account_new();
    res->key = string_new();

    res->network = network_p2p_new(config, &config_request_get, &config_request_send);

    res->full_freeze = integer_new();
    res->_is_server = 0;


    config_load(res);
    // TODO update local data
    //    network_p2p_connected(res->network);
    //    if(!account_is_null(res->account)){
    //        account_get(res->account, res->account->address);
    //        config_save_local(res);
    //    }

    return res;
}
void config_clear(struct config_st *res) {
    account_clear(res->account);
    string_clear(res->key);
    integer_clear(res->full_freeze);
    res->_is_server = 0;
}
void config_free(struct config_st *res) {
    account_free(res->account);
    string_free(res->key);
    network_p2p_free(res->network);
    integer_free(res->full_freeze);
    skr_free(res);
}

// TLV Methods
void config_set_tlv(struct config_st *res, const struct string_st *tlv) {
    if (res == NULL) return;
    config_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_CONFIG) return;
    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    network_p2p_set_hosts(res->network, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    account_set_tlv(res->account, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->key, _tlv);

    tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->full_freeze, _tlv);

    string_free(_tlv);
}
void config_get_tlv(const struct config_st *config, struct string_st *res) {
    if (res == NULL) return;
    if (config == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    network_p2p_get_hosts(config->network, res);

    account_get_tlv(config->account, tlv);
    string_concat(res, tlv);

    string_get_tlv(config->key, tlv);
    string_concat(res, tlv);

    integer_get_tlv(config->full_freeze, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_CONFIG, res);
    string_free(tlv);
}

// Local Data Methods
void config_save_local(const struct config_st *config) {
    if (config == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);

    string_set_str(path, ".data/config.skr", 19);

    config_get_tlv(config, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
void config_load(struct config_st *config) {
    if (config == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();
    string_set_str(path, ".data/config.skr", 19);

    file_read(path, tlv);
    if (tlv->size != 0) config_set_tlv(config, tlv);

    string_free(tlv);
    string_free(path);
}

// NetWork Methods
void config_server_start() {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    network_p2p_start(main_config->network);
    main_config->_is_server = 1;

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, block_checker, NULL);

    blocks_restore();

    config_account_activate();
}
void config_server_close() {
    if (main_config == NULL) main_config = config_new();
    network_p2p_close();
    main_config->_is_server = 0;
}

void config_network_get(const struct string_st *data, struct string_st *response) {
    if (main_config == NULL) main_config = config_new();
    network_p2p_get(main_config->network, data, response);
}
void config_network_send(const struct string_st *data) {
    if (main_config == NULL) main_config = config_new();
    network_p2p_send(main_config->network, data);
}


// Class Methods
void config_account_log_in(const struct string_st *login, const struct string_st *password) {
    if (main_config == NULL) main_config = config_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();
    struct string_st *key = string_new();

    get_crypto_base(1)._from_string(key, password);
    get_crypto_base(1)._get_public(key, hash);
    account_get(account, hash);

    if (string_cmp(account->address, hash) == 0 && string_cmp(account->login, login) == 0) {
        account_set(main_config->account, account);
        string_set(main_config->key, key);
        config_save_local(main_config);
    }
    account_free(account);
    string_free(hash);
    string_free(key);
}
void config_account_register(const struct string_st *login, const struct string_st *password) {
    if (main_config == NULL) main_config = config_new();
    struct string_st *key = string_new();
    struct string_st *hash = string_new();

    get_crypto_base(1)._from_string(key, password);
    cre_block_smart_account_create(login, 1, 1, 1, key);

    string_free(key);
    string_free(hash);
}
void config_account_activate() {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    cre_block_smart_account_activate(main_config->account, main_config->key);
}
void config_account_deactivate() {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    cre_block_smart_account_deactivate(main_config->account, main_config->key);
}
void config_account_username(const struct string_st *new_username) {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    if (cre_generation_account_username(new_username, main_config->account, main_config->key)) {
        string_set(main_config->account->login, new_username);
        config_save_local(main_config);
    }
}
void config_account_freeze(const struct string_st *address, const struct integer_st *balance) {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    cre_block_smart_account_freeze(address, balance, main_config->account, main_config->key);
}

void config_currency_create(const struct string_st *name, const struct string_st *smc, const struct string_st *info, const struct integer_st *our, const struct integer_st *product, const struct integer_st *hash_type, const struct integer_st *crypto_type, const struct integer_st *crypto_base, const struct integer_st *balance) {
    if (main_config == NULL) main_config = config_new();

    struct integer_st *_our = integer_new();
    struct integer_st *_product = integer_new();
    struct integer_st *_hash_type = integer_new();
    struct integer_st *_crypto_type = integer_new();
    struct integer_st *_crypto_base = integer_new();

    if (our == NULL) integer_set_ui(_our, 1);
    else integer_set(_our, our);

    if (product == NULL) integer_set_ui(_product, 0);
    else integer_set(_product, product);

    if (hash_type == NULL) integer_set_ui(_hash_type, 1);
    else integer_set(_hash_type, hash_type);

    if (crypto_type == NULL) integer_set_ui(_crypto_type, 1);
    else integer_set(_crypto_type, crypto_type);

    if (crypto_base == NULL) integer_set_ui(_crypto_base, 1);
    else integer_set(_crypto_base, crypto_base);

    if (!integer_is_null(balance) && main_config->account->address->size != 0)
        cre_block_smart_currency_create(name, main_config->account, main_config->key, smc, info, _our, _product,
                                        _hash_type, _crypto_type, _crypto_base, balance);
    else
        cre_block_smart_currency_create(name, NULL, NULL, smc, info, _our, _product, _hash_type, _crypto_type,
                                        _crypto_base, NULL);

    integer_free(_our);
    integer_free(_product);
    integer_free(_hash_type);
    integer_free(_crypto_type);
    integer_free(_crypto_base);
}

void config_wallet_create(const struct string_st *currency, const struct string_st *name, const struct string_st *smart_contract) {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;

    struct currency_st *Currency = currency_new();
    struct string_st *w_key = string_new();
    currency_get(Currency, currency);

    get_crypto_base(integer_get_ui(Currency->crypto_base))._generate(w_key);
    cre_block_smart_wallet_connect(currency, name, w_key, smart_contract, main_config->account, main_config->key);

    currency_free(Currency);
    string_free(w_key);
}
void config_wallet_delete(const struct string_st *currency, const struct string_st *address) {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    cre_block_smart_wallet_disconnect(currency, address, main_config->account, main_config->key);
}

void config_block_create(const struct block_st *block) {
    if (main_config == NULL) main_config = config_new();
    cre_generation_block_create(block);
}

void config_make_pre_transaction(struct pre_transaction *res, struct string_st *address_from, struct string_st *address_to, struct string_st *currency, struct integer_st *balance, struct integer_st *fee) {
    struct currency_st *Currency = currency_new();
    struct string_st *w_key = string_new();
    currency_get(Currency, currency);
    {
        struct wallet_smart *wallet = wallet_smart_new();
        wallet_smart_get(wallet, currency, address_from);
        wallet_smart_private(wallet, main_config->key, main_config->account, w_key);
        wallet_smart_free(wallet);
    }

    string_set(res->address_from, address_from);
    string_set(res->address_to, address_to);
    string_set(res->currency, currency);
    integer_set(res->balance, balance);
    integer_set(res->fee, fee);
    string_set(res->private_key, w_key);

    currency_free(Currency);
    string_free(w_key);
}
