#include "sakaar.h"
#include "crypto_base.h"

struct currency_create_data {
    struct integer_st *time;

    struct string_st *name;
    struct string_st *smart_contract;
    struct string_st *info;

    struct integer_st *hash_type;
    struct integer_st *crypto_type;
    struct integer_st *crypto_base;

    struct integer_st *our;
    struct integer_st *product;
    struct integer_st *balance;

    struct string_st *w_address;
    struct string_st *address;
    struct string_st *private_key;

    struct string_st *w_signature;
    struct string_st *signature;
};

struct currency_create_data *currency_create_data_new() {
    struct currency_create_data *res = skr_malloc(sizeof(struct currency_create_data));
    res->time = integer_new();

    res->name = string_new();
    res->smart_contract = string_new();
    res->info = string_new();

    res->hash_type = integer_new();
    res->crypto_type = integer_new();
    res->crypto_base = integer_new();

    res->our = integer_new();
    res->product = integer_new();
    res->balance = integer_new();

    res->w_address = string_new();
    res->address = string_new();
    res->private_key = string_new();

    res->w_signature = string_new();
    res->signature = string_new();
    return res;
}
void currency_create_data_clear(struct currency_create_data *res) {
    integer_clear(res->time);

    string_clear(res->name);
    string_clear(res->smart_contract);
    string_clear(res->info);

    integer_clear(res->hash_type);
    integer_clear(res->crypto_type);
    integer_clear(res->crypto_base);

    integer_clear(res->our);
    integer_clear(res->product);
    integer_clear(res->balance);

    string_clear(res->w_address);
    string_clear(res->address);
    string_clear(res->private_key);

    string_clear(res->w_signature);
    string_clear(res->signature);
}
void currency_create_data_free(struct currency_create_data *res) {
    integer_free(res->time);

    string_free(res->name);
    string_free(res->smart_contract);
    string_free(res->info);

    integer_free(res->hash_type);
    integer_free(res->crypto_type);
    integer_free(res->crypto_base);

    integer_free(res->our);
    integer_free(res->product);
    integer_free(res->balance);

    string_free(res->w_address);
    string_free(res->address);
    string_free(res->private_key);

    string_free(res->w_signature);
    string_free(res->signature);
    skr_free(res);
}

void currency_create_data_set_tlv(struct currency_create_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    currency_create_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_CURRENCY_CREATE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->name, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->smart_contract, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->info, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->hash_type, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->crypto_type, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->crypto_base, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->our, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->product, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->balance, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->private_key, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_signature, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void currency_create_data_get_tlv(const struct currency_create_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->name, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->smart_contract, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->info, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->hash_type, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->crypto_type, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->crypto_base, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->our, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->product, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->balance, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->private_key, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_signature, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_CURRENCY_CREATE, tlv);
    string_free(_tlv);
}
void currency_create_data_get_signature_str(const struct currency_create_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->name, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->smart_contract, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->info, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->hash_type, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->crypto_type, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->crypto_base, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->our, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->product, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->balance, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->private_key, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_currency_create(const struct string_st *smart_contract) {
    int result = 0;

    struct currency_create_data *data = currency_create_data_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();
    currency_create_data_set_tlv(data, smart_contract);

    if (currency_exist(data->name)) goto end;

    if (!integer_is_null(data->balance)) {
        if (!account_exist(data->address)) goto end;
        account_get(account, data->address);

        {
            currency_create_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address,
                                                                                   hash))
                goto end;
        }

        {
            currency_create_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(data->hash_type))._code(hash, hash);
            if (!get_crypto_base(integer_get_ui(data->crypto_base))._check_sign(data->w_signature, data->w_address,
                                                                                hash))
                goto end;
        }

    }

    result = 1;

    end:

    currency_create_data_free(data);
    account_free(account);
    string_free(hash);
    return result;
}
void cre_block_smart_currency_create(const struct string_st *name, const struct account_st *account, const struct string_st *key, const struct string_st *smc, const struct string_st *info, const struct integer_st *our, const struct integer_st *product, const struct integer_st *hash_type, const struct integer_st *crypto_type, const struct integer_st *crypto_base, const struct integer_st *balance) {
    if (string_is_null(name) || our == NULL || integer_is_null(hash_type) || integer_is_null(crypto_type) || integer_is_null(crypto_base)) return;
    if (!integer_is_null(balance) && (string_is_null(key) || account_is_null(account) || integer_is_null(our))) return;
    struct string_st *smart_contract = string_new();
    {
        struct currency_create_data *data = currency_create_data_new();

        integer_set_time(data->time);
        string_set(data->name, name);
        string_set(data->smart_contract, smc);
        string_set(data->info, info);
        integer_set(data->hash_type, hash_type);
        integer_set(data->crypto_type, crypto_type);
        integer_set(data->crypto_base, crypto_base);
        integer_set(data->our, our);
        integer_set(data->product, product);

        integer_set(data->balance, balance);
        if (!integer_is_null(balance)) {
            struct string_st *w_key = string_new();
            struct string_st *hash = string_new();
            get_crypto_base(integer_get_ui(crypto_base))._generate(w_key);


            get_crypto_base(integer_get_ui(account->crypto_base))._get_public(w_key, data->w_address);
            get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
            get_crypto_base(integer_get_ui(account->crypto_base))._encode_self(key, w_key,
                                                                               integer_get_ui(account->hash_type),
                                                                               integer_get_ui(account->crypto_type),
                                                                               data->private_key);


            {
                currency_create_data_get_signature_str(data, hash);
                get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
                get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
            }

            {
                currency_create_data_get_signature_str(data, hash);
                get_hash_code(integer_get_ui(hash_type))._code(hash, hash);
                get_crypto_base(integer_get_ui(crypto_base))._create_sign(data->w_signature, w_key, hash);
            }
            string_free(w_key);
            string_free(hash);
        }

        currency_create_data_get_tlv(data, smart_contract);
        currency_create_data_free(data);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_currency_create(const struct string_st *smart_contract) {
    struct currency_create_data *data = currency_create_data_new();
    currency_create_data_set_tlv(data, smart_contract);

    {
        struct currency_st *currency = currency_new();

        string_set(currency->name, data->name);
        string_set(currency->smart_contract, data->smart_contract);
        string_set(currency->info, data->info);
        string_set(currency->name, data->name);
        integer_set(currency->hash_type, data->hash_type);
        integer_set(currency->crypto_type, data->crypto_type);
        integer_set(currency->crypto_base, data->crypto_base);
        integer_set(currency->our, data->our);
        integer_set(currency->product, data->product);

        currency_save_local(currency);

        currency_free(currency);
    }
    if (!integer_is_null(data->balance)) {
        {
            struct wallet_st *wallet = wallet_new();

            string_set(wallet->data->address, data->w_address);
            string_set(wallet->smart->address, data->w_address);
            string_set(wallet->data->currency, data->name);
            string_set(wallet->smart->currency, data->name);

            integer_set(wallet->data->balance, data->balance);
            integer_set(wallet->data->pre_balance, data->balance);

            string_set(wallet->smart->private_key, data->private_key);
            string_set(wallet->smart->owner, data->address);
            integer_clear(wallet->smart->freeze);


            wallet_data_save_local(wallet->data);
            wallet_smart_save_local(wallet->smart);

            wallet_free(wallet);
        }
        {
            struct account_connections *connections = account_connections_new();
            account_connections_get(connections, data->name, data->address);

            if (!account_connections_contains(connections, data->w_address)) {
                account_connections_add(connections, data->w_address);
                account_connections_save_local(connections);
            }

            account_connections_free(connections);
        }
    }

    currency_create_data_free(data);
}


int block_check(const struct block_st *block);
void block_save(const struct block_st *block, int result);
void my_cre_block_smart_currency_create(const struct string_st *name, const struct account_st *account, const struct string_st *key, const struct string_st *smc, const struct string_st *info, const struct integer_st *our, const struct integer_st *product, const struct integer_st *hash_type, const struct integer_st *crypto_type, const struct integer_st *crypto_base, const struct integer_st *balance) {
    if (string_is_null(name) || our == NULL || integer_is_null(hash_type) || integer_is_null(crypto_type) || integer_is_null(crypto_base)) return;
    if (!integer_is_null(balance) && (string_is_null(key) || account_is_null(account) || integer_is_null(our))) return;
    struct string_st *smart_contract = string_new();
    {
        struct currency_create_data *data = currency_create_data_new();

        integer_set_time(data->time);
        string_set(data->name, name);
        string_set(data->smart_contract, smc);
        string_set(data->info, info);
        integer_set(data->hash_type, hash_type);
        integer_set(data->crypto_type, crypto_type);
        integer_set(data->crypto_base, crypto_base);
        integer_set(data->our, our);
        integer_set(data->product, product);

        integer_set(data->balance, balance);
        if (!integer_is_null(balance)) {
            struct string_st *w_key = string_new();
            struct string_st *hash = string_new();
            get_crypto_base(integer_get_ui(crypto_base))._generate(w_key);


            get_crypto_base(integer_get_ui(account->crypto_base))._get_public(w_key, data->w_address);
            get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
            get_crypto_base(integer_get_ui(account->crypto_base))._encode_self(key, w_key,
                                                                               integer_get_ui(account->hash_type),
                                                                               integer_get_ui(account->crypto_type),
                                                                               data->private_key);


            {
                currency_create_data_get_signature_str(data, hash);
                get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
                get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
            }

            {
                currency_create_data_get_signature_str(data, hash);
                get_hash_code(integer_get_ui(hash_type))._code(hash, hash);
                get_crypto_base(integer_get_ui(crypto_base))._create_sign(data->w_signature, w_key, hash);
            }
            string_free(w_key);
            string_free(hash);
        }

        currency_create_data_get_tlv(data, smart_contract);
        currency_create_data_free(data);
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