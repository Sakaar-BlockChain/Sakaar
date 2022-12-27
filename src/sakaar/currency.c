#include "sakaar.h"
#include <sys/stat.h>

struct object_type currency_type = {CURRENCY_OP, METHOD_GET_TLV &currency_get_tlv, METHOD_SET_TLV &currency_set_tlv};


struct currency_st *currency_new() {
    struct currency_st *res = skr_malloc(sizeof(struct currency_st));
    res->smart_contract = string_new();
    res->name = string_new();
    res->info = string_new();

    res->hash_type = integer_new();
    res->crypto_type = integer_new();
    res->crypto_base = integer_new();

    res->our = integer_new();
    res->product = integer_new();
    return res;
}
void currency_set(struct currency_st *res, const struct currency_st *a) {
    if (a == NULL) return currency_clear(res);

    string_set(res->smart_contract, a->smart_contract);
    string_set(res->name, a->name);
    string_set(res->info, a->info);

    integer_set(res->hash_type, a->hash_type);
    integer_set(res->crypto_type, a->crypto_type);
    integer_set(res->crypto_base, a->crypto_base);

    integer_set(res->our, a->our);
    integer_set(res->product, a->product);
}
void currency_clear(struct currency_st *res) {
    string_clear(res->smart_contract);
    string_clear(res->name);
    string_clear(res->info);

    integer_clear(res->hash_type);
    integer_clear(res->crypto_type);
    integer_clear(res->crypto_base);

    integer_clear(res->our);
    integer_clear(res->product);
}
void currency_free(struct currency_st *res) {
    string_free(res->smart_contract);
    string_free(res->name);
    string_free(res->info);

    integer_free(res->hash_type);
    integer_free(res->crypto_type);
    integer_free(res->crypto_base);

    integer_free(res->our);
    integer_free(res->product);
    skr_free(res);
}

void balance_outside(const struct currency_st *currency, const struct string_st *address_outside, struct integer_st *balance) {
    // TODO end this
}
void register_outside(const struct currency_st *currency, const struct string_st *address, struct string_st *address_outside) {
    // TODO end this
}
void send_outside(const struct currency_st *currency, const struct string_st *address_outside, const struct string_st *address_to, const struct integer_st *balance) {
    // TODO end this
}

void currency_get(struct currency_st *currency, const struct string_st *name) {
    if (currency == NULL) return;
    if (string_is_null(name)) return currency_clear(currency);

    if (main_config->_is_server) return currency_load(currency, name);
    return currency_request(currency, name);
}
void currency_set_tlv(struct currency_st *res, const struct string_st *tlv) {
    if (res == NULL) return;
    currency_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_CURRENCY) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->name, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->smart_contract, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->info, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->hash_type, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->crypto_type, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->crypto_base, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->our, _tlv);

    tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->product, _tlv);

    string_free(_tlv);
}
void currency_get_tlv(const struct currency_st *currency, struct string_st *res) {
    if (res == NULL) return;
    if (currency == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    string_get_tlv(currency->name, res);

    string_get_tlv(currency->smart_contract, tlv);
    string_concat(res, tlv);

    string_get_tlv(currency->info, tlv);
    string_concat(res, tlv);

    integer_get_tlv(currency->hash_type, tlv);
    string_concat(res, tlv);

    integer_get_tlv(currency->crypto_type, tlv);
    string_concat(res, tlv);

    integer_get_tlv(currency->crypto_base, tlv);
    string_concat(res, tlv);

    integer_get_tlv(currency->our, tlv);
    string_concat(res, tlv);

    integer_get_tlv(currency->product, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_CURRENCY, res);
    string_free(tlv);
}


void currency_save_local(const struct currency_st *currency) {
    if (currency == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/currency/", 15);
    mkdir(path->data, 0777);

    string_concat(path, currency->name);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    currency_get_tlv(currency, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int currency_exist(const struct string_st *name) {
    if (string_is_null(name)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/currency/", 15);
    string_concat(path, name);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    int result = file_exist(path);

    string_free(path);
    string_free(path_end);
    return result;
}
void currency_load(struct currency_st *currency, const struct string_st *name) {
    if (currency == NULL) return;
    currency_clear(currency);
    if (string_is_null(name)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/currency/", 15);
    string_concat(path, name);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) currency_set_tlv(currency, tlv);

    string_free(tlv);
    string_free(path);
}

void currency_request(struct currency_st *res, const struct string_st *name) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    currency_clear(res);

    {
        string_get_tlv(name, req);
        tlv_set_string(req, TLV_REQ_CURRENCY, req);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) currency_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void currency_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_CURRENCY) return;
    string_clear(res);

    struct string_st *name = string_new();
    struct currency_st *currency = currency_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(name, _tlv);

        string_free(_tlv);
    }
    if (currency_exist(name)) {
        currency_get(currency, name);
        currency_get_tlv(currency, res);
    }

    string_free(name);
    currency_free(currency);
}