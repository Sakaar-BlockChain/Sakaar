#include "sakaar.h"
#include <sys/stat.h>
#include "crypto_base.h"

struct object_type wallet_smart_type = {WALLET_SMART_OP, METHOD_GET_TLV &wallet_smart_get_tlv,
                                        METHOD_SET_TLV &wallet_smart_set_tlv};


struct wallet_smart *wallet_smart_new() {
    struct wallet_smart *res = skr_malloc(sizeof(struct wallet_smart));
    res->address = string_new();
    res->currency = string_new();
    res->private_key = string_new();
    res->smart_private = string_new();
    res->smart_contract = string_new();
    res->name = string_new();
    res->owner = string_new();

    res->freeze = integer_new();
    return res;
}
void wallet_smart_set(struct wallet_smart *res, const struct wallet_smart *a) {
    if (a == NULL) return wallet_smart_clear(res);

    string_set(res->address, a->address);
    string_set(res->currency, a->currency);
    string_set(res->private_key, a->private_key);
    string_set(res->smart_private, a->smart_private);
    string_set(res->smart_contract, a->smart_contract);
    string_set(res->name, a->name);
    string_set(res->owner, a->owner);

    integer_set(res->freeze, a->freeze);
}
void wallet_smart_clear(struct wallet_smart *res) {
    string_clear(res->address);
    string_clear(res->currency);
    string_clear(res->private_key);
    string_clear(res->smart_private);
    string_clear(res->smart_contract);
    string_clear(res->name);
    string_clear(res->owner);

    integer_clear(res->freeze);
}
void wallet_smart_free(struct wallet_smart *res) {
    string_free(res->address);
    string_free(res->currency);
    string_free(res->private_key);
    string_free(res->smart_private);
    string_free(res->smart_contract);
    string_free(res->name);
    string_free(res->owner);

    integer_free(res->freeze);
    skr_free(res);
}

void wallet_smart_private(const struct wallet_smart *smart, const struct string_st *private, const struct account_st *account, struct string_st *res) {
    if (res == NULL) return;
    string_clear(res);
    if (smart == NULL || string_is_null(private) || account == NULL) return;

    unsigned hash_type = integer_get_ui(account->hash_type);
    unsigned crypto_type = integer_get_ui(account->crypto_type);
    unsigned crypto_base = integer_get_ui(account->crypto_base);

    get_crypto_base(crypto_base)._decode_self(private, smart->private_key, hash_type, crypto_type, res);
}
void wallet_smart_get_contract(const struct wallet_smart *smart, const struct currency_st *currency, struct string_st *res) {
    if (res == NULL) return;
    string_clear(res);
    if (smart == NULL || currency == NULL || string_is_null(smart->smart_contract)) return string_clear(res);

    unsigned hash_type = integer_get_ui(currency->hash_type);
    unsigned crypto_type = integer_get_ui(currency->crypto_type);
    unsigned crypto_base = integer_get_ui(currency->crypto_base);

    get_crypto_base(crypto_base)._decode(smart->address, smart->smart_private, smart->smart_contract, hash_type,
                                         crypto_type, res);
}

void wallet_smart_get(struct wallet_smart *smart, const struct string_st *currency, const struct string_st *address) {
    if (smart == NULL) return;
    if (string_is_null(currency) || string_is_null(address)) return wallet_smart_clear(smart);

    if (main_config->_is_server) return wallet_smart_load(smart, currency, address);
    return wallet_smart_request(smart, currency, address);
}
void wallet_smart_set_tlv(struct wallet_smart *res, const struct string_st *tlv) {
    if (res == NULL) return;
    wallet_smart_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_WALLET_SMART) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();


    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->currency, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->private_key, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->smart_private, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->smart_contract, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->name, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->owner, _tlv);

    tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->freeze, _tlv);

    string_free(_tlv);
}
void wallet_smart_get_tlv(const struct wallet_smart *smart, struct string_st *res) {
    if (res == NULL) return;
    if (smart == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    string_get_tlv(smart->address, res);

    string_get_tlv(smart->currency, tlv);
    string_concat(res, tlv);

    string_get_tlv(smart->private_key, tlv);
    string_concat(res, tlv);

    string_get_tlv(smart->smart_private, tlv);
    string_concat(res, tlv);

    string_get_tlv(smart->smart_contract, tlv);
    string_concat(res, tlv);

    string_get_tlv(smart->name, tlv);
    string_concat(res, tlv);

    string_get_tlv(smart->owner, tlv);
    string_concat(res, tlv);

    integer_get_tlv(smart->freeze, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_WALLET_SMART, res);
    string_free(tlv);
}


void wallet_smart_save_local(const struct wallet_smart *smart) {
    if (smart == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/wallet_smart/", 19);
    mkdir(path->data, 0777);
    string_concat(path, smart->currency);
    string_set_str(tlv, "/", 1);
    string_concat(path, tlv);
    mkdir(path->data, 0777);

    tlv_beautify(smart->address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    wallet_smart_get_tlv(smart, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
void wallet_smart_load(struct wallet_smart *smart, const struct string_st *currency, const struct string_st *address) {
    if (smart == NULL) return;
    wallet_smart_clear(smart);
    if (string_is_null(currency) || string_is_null(address)) return;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/wallet_smart/", 19);
    string_concat(path, currency);
    string_set_str(path_end, "/", 1);
    string_concat(path, path_end);
    tlv_beautify(address, path_end);
    string_concat(path, path_end);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    file_read(path, tlv);
    if (tlv->size != 0) wallet_smart_set_tlv(smart, tlv);
    else {
        string_set(smart->address, address);
        string_set(smart->currency, currency);
    }

    string_free(tlv);
    string_free(path);
    string_free(path_end);
}

void wallet_smart_request(struct wallet_smart *res, const struct string_st *currency, const struct string_st *address) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    wallet_smart_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(currency, req);

        string_get_tlv(address, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_WALLET_SMART, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) wallet_smart_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void wallet_smart_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_WALLET_SMART) return;
    string_clear(res);

    struct string_st *currency = string_new();
    struct string_st *address = string_new();
    struct wallet_smart *smart = wallet_smart_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(currency, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(address, _tlv);

        string_free(_tlv);
    }
    wallet_smart_get(smart, currency, address);
    wallet_smart_get_tlv(smart, res);

    string_free(currency);
    string_free(address);
    wallet_smart_free(smart);
}
