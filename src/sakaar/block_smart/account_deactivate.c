#include "sakaar.h"
#include "crypto_base.h"

struct account_deactivate_data {
    struct integer_st *time;
    struct string_st *address;
    struct string_st *signature;
};

struct account_deactivate_data *account_deactivate_data_new() {
    struct account_deactivate_data *res = skr_malloc(sizeof(struct account_deactivate_data));
    res->time = integer_new();
    res->address = string_new();
    res->signature = string_new();
    return res;
}
void account_deactivate_data_clear(struct account_deactivate_data *res) {
    integer_clear(res->time);
    string_clear(res->address);
    string_clear(res->signature);
}
void account_deactivate_data_free(struct account_deactivate_data *res) {
    integer_free(res->time);
    string_free(res->address);
    string_free(res->signature);
    skr_free(res);
}

void account_deactivate_data_set_tlv(struct account_deactivate_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    account_deactivate_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_ACCOUNT_DEACTIVATE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void account_deactivate_data_get_tlv(const struct account_deactivate_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_ACCOUNT_DEACTIVATE, tlv);
    string_free(_tlv);
}
void account_deactivate_data_get_signature_str(const struct account_deactivate_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_account_deactivate(const struct string_st *smart_contract) {
    int result = 0;

    struct activated_accounts *activated = activated_accounts_new();
    struct account_deactivate_data *data = account_deactivate_data_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();

    account_deactivate_data_set_tlv(data, smart_contract);

    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    {
        account_deactivate_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    activated_accounts_get(activated);
    if (!activated_accounts_contains(activated, data->address) || integer_is_null(account->activated)) goto end;

    result = 1;

    end:

    activated_accounts_free(activated);
    account_deactivate_data_free(data);
    account_free(account);
    string_free(hash);
    return result;
}
void cre_block_smart_account_deactivate(const struct account_st *account, const struct string_st *key) {
    if (string_is_null(key) || account_is_null(account)) return;
    struct string_st *smart_contract = string_new();
    {
        struct account_deactivate_data *data = account_deactivate_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        {
            account_deactivate_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        account_deactivate_data_get_tlv(data, smart_contract);
        account_deactivate_data_free(data);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_account_deactivate(const struct string_st *smart_contract) {
    struct account_deactivate_data *data = account_deactivate_data_new();
    account_deactivate_data_set_tlv(data, smart_contract);

    {
        struct account_st *account = account_new();
        account_get(account, data->address);
        integer_clear(account->activated);
        account_save_local(account);
        account_free(account);
    }
    {
        struct activated_accounts *activated = activated_accounts_new();
        activated_accounts_get(activated);
        activated_accounts_remove(activated, data->address);
        activated_accounts_save_local(activated);
        activated_accounts_free(activated);
    }
    if (!account_is_null(main_config->account) && string_cmp(main_config->account->address, data->address) == 0) {
        integer_clear(main_config->account->activated);
        config_save_local(main_config);
    }


    account_deactivate_data_free(data);
}


