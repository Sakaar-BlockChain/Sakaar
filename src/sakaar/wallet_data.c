#include "sakaar.h"

// Local Data Methods
void wallet_data_get(struct wallet_data *data, const struct string_st *currency, const struct string_st *address) {
    if (data == NULL) return;
    if (string_is_null(currency) || string_is_null(address)) return wallet_data_clear(data);

    if (main_config->_is_server) {
        if (wallet_data_exist(currency, address))
            return wallet_data_load(data, currency, address);
        if (cre_generation_wallet_create(currency, address))
            return wallet_data_load(data, currency, address);
        return;
    }
    return wallet_data_request(data, currency, address);
}
void wallet_data_save_local(const struct wallet_data *data) {
    if (data == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/wallet_data/", 18);
    mkdir(path->data, 0777);
    string_concat(path, data->currency);
    string_set_str(tlv, "/", 1);
    string_concat(path, tlv);
    mkdir(path->data, 0777);

    tlv_beautify(data->address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    wallet_data_get_tlv(data, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int wallet_data_exist(const struct string_st *currency, const struct string_st *address) {
    if (string_is_null(currency) || string_is_null(address)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/wallet_data/", 18);
    string_concat(path, currency);
    string_set_str(path_end, "/", 1);
    string_concat(path, path_end);
    tlv_beautify(address, path_end);
    string_concat(path, path_end);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    int result = file_exist(path);

    string_free(path);
    string_free(path_end);
    return result;
}
void wallet_data_load(struct wallet_data *data, const struct string_st *currency, const struct string_st *address) {
    if (data == NULL) return;
    wallet_data_clear(data);
    if (string_is_null(currency) || string_is_null(address)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/wallet_data/", 18);
    string_concat(path, currency);
    string_set_str(tlv, "/", 1);
    string_concat(path, tlv);
    tlv_beautify(address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) wallet_data_set_tlv(data, tlv);

    string_free(tlv);
    string_free(path);
}

// NetWork Methods
void wallet_data_request(struct wallet_data *res, const struct string_st *currency, const struct string_st *address) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    wallet_data_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(currency, req);

        string_get_tlv(address, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_WALLET_DATA, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) wallet_data_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void wallet_data_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_WALLET_DATA) return;
    string_clear(res);

    struct string_st *currency = string_new();
    struct string_st *address = string_new();
    struct wallet_data *data = wallet_data_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(currency, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(address, _tlv);

        string_free(_tlv);
    }
    wallet_data_get(data, currency, address);
    wallet_data_get_tlv(data, res);

    string_free(currency);
    string_free(address);
    wallet_data_free(data);
}
