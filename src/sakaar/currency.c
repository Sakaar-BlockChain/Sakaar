#include "sakaar.h"

// Local Data Methods
void currency_get(struct currency_st *currency, const struct string_st *name) {
    if (currency == NULL) return;
    if (string_is_null(name)) return currency_clear(currency);

    if (main_config->_is_server) return currency_load(currency, name);
    return currency_request(currency, name);
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

// NetWork Methods
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

// Class Methods
void balance_outside(const struct currency_st *currency, const struct string_st *address_outside, struct integer_st *balance) {
    // TODO end this
}
void register_outside(const struct currency_st *currency, const struct string_st *address, struct string_st *address_outside) {
    // TODO end this
}
void send_outside(const struct currency_st *currency, const struct string_st *address_outside, const struct string_st *address_to, const struct integer_st *balance) {
    // TODO end this
}
