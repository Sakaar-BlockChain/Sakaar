#include "sakaar.h"
#include "crypto_base.h"

struct account_create_data {
    struct integer_st *time;

    struct string_st *address;
    struct string_st *login;

    struct integer_st *hash_type;
    struct integer_st *crypto_type;
    struct integer_st *crypto_base;

    struct string_st *signature;
};

struct account_create_data *account_create_data_new() {
    struct account_create_data *res = skr_malloc(sizeof(struct account_create_data));
    res->time = integer_new();

    res->address = string_new();
    res->login = string_new();

    res->hash_type = integer_new();
    res->crypto_type = integer_new();
    res->crypto_base = integer_new();

    res->signature = string_new();
    return res;
}
void account_create_data_clear(struct account_create_data *res) {
    integer_clear(res->time);

    string_clear(res->address);
    string_clear(res->login);

    integer_clear(res->hash_type);
    integer_clear(res->crypto_type);
    integer_clear(res->crypto_base);

    string_clear(res->signature);
}
void account_create_data_free(struct account_create_data *res) {
    integer_free(res->time);

    string_free(res->address);
    string_free(res->login);

    integer_free(res->hash_type);
    integer_free(res->crypto_type);
    integer_free(res->crypto_base);

    string_free(res->signature);
    skr_free(res);
}

void account_create_data_set_tlv(struct account_create_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
//    account_create_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_ACCOUNT_CREATE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->login, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->hash_type, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->crypto_type, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->crypto_base, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void account_create_data_get_tlv(const struct account_create_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->login, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->hash_type, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->crypto_type, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->crypto_base, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_ACCOUNT_CREATE, tlv);
    string_free(_tlv);
}
void account_create_data_get_signature_str(const struct account_create_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->login, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->hash_type, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->crypto_type, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->crypto_base, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_account_create(const struct string_st *smart_contract) {
    int result = 0;

    struct account_create_data *data = account_create_data_new();
    struct string_st *hash = string_new();
    account_create_data_set_tlv(data, smart_contract);

    {
        account_create_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(data->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(data->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }
    if (account_exist(data->address)) goto end;

    result = 1;

    end:

    account_create_data_free(data);
    string_free(hash);
    return result;
}
void cre_block_smart_account_create(const struct string_st *login, int hash_type, int crypto_type, int crypto_base, const struct string_st *key) {
    if (string_is_null(key)) return;
    struct string_st *smart_contract = string_new();
    {
        struct account_create_data *data = account_create_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        get_crypto_base(crypto_base)._get_public(key, data->address);
        string_set(data->login, login);
        integer_set_ui(data->hash_type, hash_type);
        integer_set_ui(data->crypto_type, crypto_type);
        integer_set_ui(data->crypto_base, crypto_base);
        {
            account_create_data_get_signature_str(data, hash);
            get_hash_code(hash_type)._code(hash, hash);
            get_crypto_base(crypto_base)._create_sign(data->signature, key, hash);

        }

        account_create_data_get_tlv(data, smart_contract);
        account_create_data_free(data);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_account_create(const struct string_st *smart_contract) {
    struct account_create_data *data = account_create_data_new();
    account_create_data_set_tlv(data, smart_contract);

    {
        struct account_st *account = account_new();
        string_set(account->address, data->address);
        string_set(account->login, data->login);

        integer_clear(account->activated);
        integer_clear(account->freeze);

        integer_set(account->hash_type, data->hash_type);
        integer_set(account->crypto_type, data->crypto_type);
        integer_set(account->crypto_base, data->crypto_base);

        account_save_local(account);
        account_free(account);
    }


    account_create_data_free(data);
}


int block_check(const struct block_st *block);
void block_save(const struct block_st *block, int result);
void my_cre_block_smart_account_create(const struct string_st *login, int hash_type, int crypto_type, int crypto_base, const struct string_st *key) {
    if (string_is_null(key)) return;
    struct string_st *smart_contract = string_new();
    {
        struct account_create_data *data = account_create_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        get_crypto_base(crypto_base)._get_public(key, data->address);
        string_set(data->login, login);
        integer_set_ui(data->hash_type, hash_type);
        integer_set_ui(data->crypto_type, crypto_type);
        integer_set_ui(data->crypto_base, crypto_base);
        {
            account_create_data_get_signature_str(data, hash);
            get_hash_code(hash_type)._code(hash, hash);
            get_crypto_base(crypto_base)._create_sign(data->signature, key, hash);

        }

        account_create_data_get_tlv(data, smart_contract);
        account_create_data_free(data);
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
