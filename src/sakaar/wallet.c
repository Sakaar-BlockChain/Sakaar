#include "sakaar.h"

struct object_type wallet_type = {WALLET_OP, METHOD_GET_TLV &wallet_get_tlv, METHOD_SET_TLV &wallet_set_tlv};


struct wallet_st *wallet_new() {
    struct wallet_st *res = skr_malloc(sizeof(struct wallet_st));
    res->data = wallet_data_new();
    res->smart = wallet_smart_new();
    return res;
}
void wallet_set(struct wallet_st *res, const struct wallet_st *a) {
    if (a == NULL) return wallet_clear(res);

    wallet_data_set(res->data, a->data);
    wallet_smart_set(res->smart, a->smart);
}
void wallet_clear(struct wallet_st *res) {
    wallet_data_clear(res->data);
    wallet_smart_clear(res->smart);
}
void wallet_free(struct wallet_st *res) {
    wallet_data_free(res->data);
    wallet_smart_free(res->smart);
    skr_free(res);
}

void wallet_get(struct wallet_st *wallet, const struct string_st *currency, const struct string_st *address) {
    if (wallet == NULL) return;
    if (string_is_null(currency) || string_is_null(address)) return wallet_clear(wallet);

    if (main_config->_is_server) {
        wallet_data_get(wallet->data, currency, address);
        wallet_smart_get(wallet->smart, currency, address);
        return;
    }
    return wallet_request(wallet, currency, address);
}
void wallet_set_tlv(struct wallet_st *res, const struct string_st *tlv) {
    if (res == NULL) return;
    wallet_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_WALLET) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    wallet_data_set_tlv(res->data, _tlv);

    tlv_get_next_tlv(data, _tlv);
    wallet_smart_set_tlv(res->smart, _tlv);

    string_free(_tlv);
}
void wallet_get_tlv(const struct wallet_st *wallet, struct string_st *res) {
    if (res == NULL) return;
    if (wallet == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    wallet_data_get_tlv(wallet->data, res);

    wallet_smart_get_tlv(wallet->smart, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_WALLET, res);
    string_free(tlv);
}


void wallet_request(struct wallet_st *res, const struct string_st *currency, const struct string_st *address) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    wallet_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(currency, req);

        string_get_tlv(address, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_WALLET, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) wallet_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void wallet_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_WALLET) return;
    string_clear(res);

    struct string_st *currency = string_new();
    struct string_st *address = string_new();
    struct wallet_st *wallet = wallet_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(currency, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(address, _tlv);

        string_free(_tlv);
    }
    wallet_get(wallet, currency, address);
    wallet_get_tlv(wallet, res);

    string_free(currency);
    string_free(address);
    wallet_free(wallet);
}