#include "sakaar.h"
#include "crypto_base.h"

struct account_username_data {
    struct integer_st *time;
    struct string_st *login;
    struct string_st *address;
    struct string_st *signature;
};

struct account_username_data *account_username_data_new() {
    struct account_username_data *res = skr_malloc(sizeof(struct account_username_data));
    res->time = integer_new();
    res->login = string_new();
    res->address = string_new();
    res->signature = string_new();
    return res;
}
void account_username_data_clear(struct account_username_data *res) {
    integer_clear(res->time);
    string_clear(res->login);
    string_clear(res->address);
    string_clear(res->signature);
}
void account_username_data_free(struct account_username_data *res) {
    integer_free(res->time);
    string_free(res->login);
    string_free(res->address);
    string_free(res->signature);
    skr_free(res);
}

void account_username_data_set_tlv(struct account_username_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    account_username_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_GEN_ACCOUNT_USERNAME) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->login, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void account_username_data_get_tlv(const struct account_username_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->login, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_GEN_ACCOUNT_USERNAME, tlv);
    string_free(_tlv);
}
void account_username_data_get_signature_str(const struct account_username_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->login, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_generation_account_username(const struct generation *res) {
    int result = 0;

    struct account_username_data *data = account_username_data_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();

    account_username_data_set_tlv(data, res->data);


    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    {
        account_username_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    if (string_cmp(data->login, account->login) == 0) goto end;

    result = 1;

    end:

    account_username_data_free(data);
    account_free(account);
    string_free(hash);
    return result;
}
int cre_generation_account_username(const struct string_st *login, const struct account_st *account, const struct string_st *key) {
    if (string_is_null(key) || account_is_null(account)) return 0;
    struct generation *res = generation_new();
    {
        struct account_username_data *data = account_username_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        string_set(data->login, login);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        {
            account_username_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        account_username_data_get_tlv(data, res->data);
        account_username_data_free(data);
        string_free(hash);
    }
    generate_make_hash(res);
    int _result = generation_work(res);
    if (_result) {
        struct string_st *msg = string_new();
        generation_get_tlv(res, msg);
        config_network_send(msg);
        string_free(msg);
    }
    generation_free(res);
    return _result;
}
void mk_generation_account_username(const struct generation *res) {
    struct account_username_data *data = account_username_data_new();
    account_username_data_set_tlv(data, res->data);

    {
        struct account_st *account = account_new();
        account_get(account, data->address);
        string_set(account->login, data->login);
        account_save_local(account);
        account_free(account);
    }

    account_username_data_free(data);
}


