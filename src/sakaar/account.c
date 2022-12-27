#include "sakaar.h"
#include <sys/stat.h>

struct object_type account_type = {ACCOUNT_OP, METHOD_GET_TLV &account_get_tlv, METHOD_SET_TLV &account_set_tlv};


struct account_st *account_new() {
    struct account_st *res = skr_malloc(sizeof(struct account_st));
    res->address = string_new();
    res->login = string_new();

    res->activated = integer_new();
    res->freeze = integer_new();

    res->hash_type = integer_new();
    res->crypto_type = integer_new();
    res->crypto_base = integer_new();
    return res;
}
void account_set(struct account_st *res, const struct account_st *a) {
    if (a == NULL) return account_clear(res);

    string_set(res->address, a->address);
    string_set(res->login, a->login);

    integer_set(res->activated, a->activated);
    integer_set(res->freeze, a->freeze);

    integer_set(res->hash_type, a->hash_type);
    integer_set(res->crypto_type, a->crypto_type);
    integer_set(res->crypto_base, a->crypto_base);
}
void account_clear(struct account_st *res) {
    string_clear(res->address);
    string_clear(res->login);

    integer_clear(res->activated);
    integer_clear(res->freeze);

    integer_clear(res->hash_type);
    integer_clear(res->crypto_type);
    integer_clear(res->crypto_base);
}
void account_free(struct account_st *res) {
    string_free(res->address);
    string_free(res->login);

    integer_free(res->activated);
    integer_free(res->freeze);

    integer_free(res->hash_type);
    integer_free(res->crypto_type);
    integer_free(res->crypto_base);
    skr_free(res);
}
int account_is_null(const struct account_st *a) {
    return (a == NULL || string_is_null(a->address));
}

void account_get(struct account_st *account, const struct string_st *address) {
    if (account == NULL) return;
    if (string_is_null(address)) return account_clear(account);

    if (main_config->_is_server) return account_load(account, address);
    return account_request(account, address);
}
void account_set_tlv(struct account_st *res, const struct string_st *tlv) {
    if (res == NULL) return;
    account_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_ACCOUNT) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->login, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->activated, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->freeze, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->hash_type, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->crypto_type, _tlv);

    tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->crypto_base, _tlv);

    string_free(_tlv);
}
void account_get_tlv(const struct account_st *account_conn, struct string_st *res) {
    if (res == NULL) return;
    if (account_conn == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    string_get_tlv(account_conn->address, res);

    string_get_tlv(account_conn->login, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->activated, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->freeze, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->hash_type, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->crypto_type, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->crypto_base, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_ACCOUNT, res);
    string_free(tlv);
}


void account_save_local(const struct account_st *account) {
    if (account == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/account/", 14);
    mkdir(path->data, 0777);

    tlv_beautify(account->address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    account_get_tlv(account, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int account_exist(const struct string_st *address) {
    if (string_is_null(address)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/account/", 14);
    tlv_beautify(address, path_end);
    string_concat(path, path_end);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    int result = file_exist(path);

    string_free(path);
    string_free(path_end);
    return result;
}
void account_load(struct account_st *account, const struct string_st *address) {
    if (account == NULL) return;
    account_clear(account);
    if (string_is_null(address)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/account/", 14);
    tlv_beautify(address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->data != NULL) account_set_tlv(account, tlv);

    string_free(tlv);
    string_free(path);
}

void account_request(struct account_st *res, const struct string_st *address) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    account_clear(res);

    {
        string_get_tlv(address, req);
        tlv_set_string(req, TLV_REQ_ACCOUNT, req);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) account_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void account_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_ACCOUNT) return;
    string_clear(res);

    struct string_st *address = string_new();
    struct account_st *account = account_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(address, _tlv);

        string_free(_tlv);
    }
    if (account_exist(address)) {
        account_get(account, address);
        account_get_tlv(account, res);
    }

    string_free(address);
    account_free(account);
}
