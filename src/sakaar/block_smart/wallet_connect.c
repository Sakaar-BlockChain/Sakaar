#include "sakaar.h"
#include "crypto_base.h"

struct wallet_connect_data {
    struct integer_st *time;

    struct string_st *address;
    struct string_st *w_address;
    struct string_st *currency;
    struct string_st *name;
    // Smart Contract
    struct string_st *smart_contract;
    struct string_st *smart_hash;
    struct string_st *smart_private;
    // Private key
    struct string_st *private_key;

    struct string_st *w_signature;
    struct string_st *signature;
};

struct wallet_connect_data *wallet_connect_data_new() {
    struct wallet_connect_data *res = skr_malloc(sizeof(struct wallet_connect_data));
    res->time = integer_new();

    res->address = string_new();
    res->w_address = string_new();
    res->currency = string_new();
    res->name = string_new();

    res->smart_contract = string_new();
    res->smart_hash = string_new();
    res->smart_private = string_new();

    res->private_key = string_new();

    res->w_signature = string_new();
    res->signature = string_new();
    return res;
}
void wallet_connect_data_clear(struct wallet_connect_data *res) {
    integer_clear(res->time);

    string_clear(res->address);
    string_clear(res->w_address);
    string_clear(res->currency);
    string_clear(res->name);

    string_clear(res->smart_contract);
    string_clear(res->smart_hash);
    string_clear(res->smart_private);

    string_clear(res->private_key);

    string_clear(res->w_signature);
    string_clear(res->signature);
}
void wallet_connect_data_free(struct wallet_connect_data *res) {
    integer_free(res->time);

    string_free(res->address);
    string_free(res->w_address);
    string_free(res->currency);
    string_free(res->name);

    string_free(res->smart_contract);
    string_free(res->smart_hash);
    string_free(res->smart_private);

    string_free(res->private_key);

    string_free(res->w_signature);
    string_free(res->signature);
    skr_free(res);
}

void wallet_connect_data_set_tlv(struct wallet_connect_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    wallet_connect_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_WALLET_CONNECT) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->currency, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->name, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->smart_contract, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->smart_hash, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->smart_private, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->private_key, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_signature, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void wallet_connect_data_get_tlv(const struct wallet_connect_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->currency, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->name, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->smart_contract, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->smart_hash, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->smart_private, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->private_key, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_signature, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_WALLET_CONNECT, tlv);
    string_free(_tlv);
}
void wallet_connect_data_get_signature_str(const struct wallet_connect_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->currency, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->name, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->smart_contract, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->smart_hash, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->smart_private, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->private_key, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_wallet_connect(const struct string_st *smart_contract) {
    int result = 0;

    struct wallet_connect_data *data = wallet_connect_data_new();
    struct currency_st *currency = currency_new();
    struct wallet_st *wallet = wallet_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();

    wallet_connect_data_set_tlv(data, smart_contract);

    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    if (!currency_exist(data->currency)) goto end;
    currency_get(currency, data->currency);
    wallet_get(wallet, data->currency, data->w_address);

    {
        wallet_connect_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    {
        wallet_connect_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(currency->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(currency->crypto_base))._check_sign(data->w_signature, data->w_address,
                                                                                hash))
            goto end;
    }

    if (!string_is_null(data->smart_hash)) {
        unsigned hash_type = integer_get_ui(currency->hash_type);
        unsigned crypto_type = integer_get_ui(currency->crypto_type);
        unsigned crypto_base = integer_get_ui(currency->crypto_base);

        get_crypto_base(crypto_base)._decode(data->w_address, data->smart_private, data->smart_contract, hash_type,
                                             crypto_type, hash);
        get_hash_code(hash_type)._code(hash, hash);
        if (string_cmp(hash, data->smart_hash) != 0) goto end;
    }

    if (wallet->smart->owner->size != 0 && string_cmp(wallet->smart->owner, data->address) != 0) goto end;

    result = 1;

    end:

    wallet_connect_data_free(data);
    currency_free(currency);
    wallet_free(wallet);
    account_free(account);
    string_free(hash);
    return result;
}
void cre_block_smart_wallet_connect(const struct string_st *currency, const struct string_st *name, const struct string_st *w_key, const struct string_st *smc, const struct account_st *account, const struct string_st *key) {
    if (string_is_null(currency) || string_is_null(w_key) || string_is_null(key) || account_is_null(account)) return;
    if (!currency_exist(currency)) return;
    struct string_st *smart_contract = string_new();
    {
        struct wallet_connect_data *data = wallet_connect_data_new();
        struct currency_st *Currency = currency_new();
        struct string_st *hash = string_new();
        currency_get(Currency, currency);


        integer_set_time(data->time);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        get_crypto_base(integer_get_ui(Currency->crypto_base))._get_public(w_key, data->w_address);
        string_set(data->currency, currency);
        string_set(data->name, name);
        if (!string_is_null(smc)) {
            unsigned hash_type = integer_get_ui(Currency->hash_type);
            unsigned crypto_type = integer_get_ui(Currency->crypto_type);
            unsigned crypto_base = integer_get_ui(Currency->crypto_base);

            get_crypto_base(crypto_base)._generate(data->smart_private);
            get_crypto_base(crypto_base)._encode(data->w_address, data->smart_private, smc, hash_type, crypto_type,
                                                 data->smart_contract);
            get_hash_code(hash_type)._code(data->smart_hash, smc);
        }
        {
            unsigned hash_type = integer_get_ui(account->hash_type);
            unsigned crypto_type = integer_get_ui(account->crypto_type);
            unsigned crypto_base = integer_get_ui(account->crypto_base);

            get_crypto_base(crypto_base)._encode_self(key, w_key, hash_type, crypto_type, data->private_key);
        }

        {
            wallet_connect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        {
            wallet_connect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(Currency->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(Currency->crypto_base))._create_sign(data->w_signature, w_key, hash);
        }


        wallet_connect_data_get_tlv(data, smart_contract);
        wallet_connect_data_free(data);
        currency_free(Currency);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_wallet_connect(const struct string_st *smart_contract) {
    struct wallet_connect_data *data = wallet_connect_data_new();
    wallet_connect_data_set_tlv(data, smart_contract);

    {
        struct wallet_smart *smart = wallet_smart_new();
        wallet_smart_get(smart, data->currency, data->w_address);

        string_set(smart->private_key, data->private_key);
        string_set(smart->name, data->name);
        string_set(smart->owner, data->address);
        integer_clear(smart->freeze);

        if (!string_is_null(data->smart_hash)) {
            string_set(smart->smart_private, data->smart_private);
            string_set(smart->smart_contract, data->smart_contract);
        }

        wallet_smart_save_local(smart);

        wallet_smart_free(smart);
    }
    {
        struct account_connections *connections = account_connections_new();
        account_connections_get(connections, data->currency, data->address);

        if (!account_connections_contains(connections, data->w_address)) {
            account_connections_add(connections, data->w_address);
            account_connections_save_local(connections);
        }

        account_connections_free(connections);
    }


    wallet_connect_data_free(data);
}


int block_check(const struct block_st *block);
void block_save(const struct block_st *block, int result);
void my_cre_block_smart_wallet_connect(const struct string_st *currency, const struct string_st *name, const struct string_st *w_key, const struct string_st *smc, const struct account_st *account, const struct string_st *key) {
    if (string_is_null(currency) || string_is_null(w_key) || string_is_null(key) || account_is_null(account)) return;
    if (!currency_exist(currency)) return;
    struct string_st *smart_contract = string_new();
    {
        struct wallet_connect_data *data = wallet_connect_data_new();
        struct currency_st *Currency = currency_new();
        struct string_st *hash = string_new();
        currency_get(Currency, currency);


        integer_set_time(data->time);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        get_crypto_base(integer_get_ui(Currency->crypto_base))._get_public(w_key, data->w_address);
        string_set(data->currency, currency);
        string_set(data->name, name);
        if (!string_is_null(smc)) {
            unsigned hash_type = integer_get_ui(Currency->hash_type);
            unsigned crypto_type = integer_get_ui(Currency->crypto_type);
            unsigned crypto_base = integer_get_ui(Currency->crypto_base);

            get_crypto_base(crypto_base)._generate(data->smart_private);
            get_crypto_base(crypto_base)._encode(data->w_address, data->smart_private, smc, hash_type, crypto_type,
                                                 data->smart_contract);
            get_hash_code(hash_type)._code(data->smart_hash, smc);
        }
        {
            unsigned hash_type = integer_get_ui(account->hash_type);
            unsigned crypto_type = integer_get_ui(account->crypto_type);
            unsigned crypto_base = integer_get_ui(account->crypto_base);

            get_crypto_base(crypto_base)._encode_self(key, w_key, hash_type, crypto_type, data->private_key);
        }

        {
            wallet_connect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        {
            wallet_connect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(Currency->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(Currency->crypto_base))._create_sign(data->w_signature, w_key, hash);
        }


        wallet_connect_data_get_tlv(data, smart_contract);
        wallet_connect_data_free(data);
        currency_free(Currency);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);
    int result = block_check(block);
    printf("result : %d\n", result);
    if (result) block_save(block, 1);

    string_free(smart_contract);
    block_free(block);
}