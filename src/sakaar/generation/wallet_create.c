#include "sakaar.h"

struct wallet_create_data {
    struct string_st *address;
    struct string_st *currency;
    struct string_st *address_outside;
};

struct wallet_create_data *wallet_create_data_new() {
    struct wallet_create_data *res = skr_malloc(sizeof(struct wallet_create_data));
    res->address = string_new();
    res->currency = string_new();
    res->address_outside = string_new();
    return res;
}
void wallet_create_data_clear(struct wallet_create_data *res) {
    string_clear(res->address);
    string_clear(res->currency);
    string_clear(res->address_outside);
}
void wallet_create_data_free(struct wallet_create_data *res) {
    string_free(res->address);
    string_free(res->currency);
    string_free(res->address_outside);
    skr_free(res);
}

void wallet_create_data_set_tlv(struct wallet_create_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    wallet_create_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_WALLET_CREATE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->currency, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address_outside, _tlv);

    string_free(_tlv);
}
void wallet_create_data_get_tlv(const struct wallet_create_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    string_get_tlv(data->currency, tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->address_outside, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_WALLET_CREATE, tlv);
    string_free(_tlv);
}

int che_generation_wallet_create(const struct generation *res) {
    int result = 0;

    struct wallet_create_data *data = wallet_create_data_new();

    wallet_create_data_set_tlv(data, res->data);

    {
        if (!currency_exist(data->currency)) goto end;
        if (wallet_data_exist(data->currency, data->address)) goto end;
        result = 1;
    }

    end:

    wallet_create_data_free(data);
    return result;
}
int cre_generation_wallet_create(const struct string_st *currency, const struct string_st *address) {
    if (string_is_null(currency) || string_is_null(address) || !currency_exist(currency)) return 0;
    struct generation *res = generation_new();
    {
        struct currency_st *Currency = currency_new();
        struct wallet_create_data *data = wallet_create_data_new();
        currency_get(Currency, currency);

        string_set(data->currency, currency);
        string_set(data->address, address);
        if (integer_is_null(Currency->our)) register_outside(Currency, address, data->address_outside);

        wallet_create_data_get_tlv(data, res->data);
        wallet_create_data_free(data);
        currency_free(Currency);
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
void mk_generation_wallet_create(const struct generation *res) {
    struct wallet_create_data *data = wallet_create_data_new();
    wallet_create_data_set_tlv(data, res->data);

    {
        struct wallet_st *wallet = wallet_new();

        string_set(wallet->data->address, data->address);
        string_set(wallet->smart->address, data->address);
        string_set(wallet->data->currency, data->currency);
        string_set(wallet->smart->currency, data->currency);
        string_set(wallet->data->address_outside, data->address_outside);

        wallet_data_save_local(wallet->data);
        wallet_smart_save_local(wallet->smart);

        wallet_free(wallet);
    }

    wallet_create_data_free(data);
}
