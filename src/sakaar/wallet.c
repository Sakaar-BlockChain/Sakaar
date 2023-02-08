#include "sakaar.h"

// Local Data Methods
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

// NetWork Methods
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
