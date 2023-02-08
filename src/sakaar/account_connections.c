#include "sakaar.h"

// Local Data Methods
void account_connections_get(struct account_connections *account_conn, const struct string_st *currency, const struct string_st *address) {
    if (account_conn == NULL) return;
    if (string_is_null(currency) || string_is_null(address)) return account_connections_clear(account_conn);

    if (main_config->_is_server) return account_connections_load(account_conn, currency, address);
    return account_connections_request(account_conn, currency, address);
}
void account_connections_save_local(const struct account_connections *accountConnections) {
    if (accountConnections == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/account/", 14);
    mkdir(path->data, 0777);
    tlv_beautify(accountConnections->address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "/", 1);
    string_concat(path, tlv);
    mkdir(path->data, 0777);

    string_concat(path, accountConnections->currency);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    account_connections_get_tlv(accountConnections, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
void account_connections_load(struct account_connections *accountConnections, const struct string_st *currency, const struct string_st *address) {
    if (accountConnections == NULL) return;
    account_connections_clear(accountConnections);
    if (string_is_null(currency) || string_is_null(address)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/account/", 14);
    tlv_beautify(address, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "/", 1);
    string_concat(path, tlv);
    string_concat(path, currency);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) account_connections_set_tlv(accountConnections, tlv);
    else {
        string_set(accountConnections->address, address);
        string_set(accountConnections->currency, currency);
    }

    string_free(tlv);
    string_free(path);
}

// NetWork Methods
void account_connections_request(struct account_connections *res, const struct string_st *currency, const struct string_st *address) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    account_connections_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(currency, req);

        string_get_tlv(address, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_ACCOUNT_CONN, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) account_connections_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void account_connections_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_ACCOUNT_CONN) return;
    string_clear(res);

    struct string_st *currency = string_new();
    struct string_st *address = string_new();
    struct account_connections *account_conn = account_connections_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(currency, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(address, _tlv);

        string_free(_tlv);
    }
    account_connections_get(account_conn, currency, address);
    account_connections_get_tlv(account_conn, res);

    string_free(currency);
    string_free(address);
    account_connections_free(account_conn);
}

// Class Methods
void account_connections_add(struct account_connections *res, const struct string_st *address) {
    if (res == NULL || string_is_null(address)) return;

    list_add_new(res->addresses, STRING_TYPE);
    string_set(res->addresses->data[res->addresses->size - 1]->data, address);
}
void account_connections_remove(struct account_connections *res, const struct string_st *address) {
    if (res == NULL || string_is_null(address)) return;

    struct list_st *list = list_new();
    list_resize(list, res->addresses->size);
    size_t size = 0;
    for (size_t i = 0; i < res->addresses->size; i++) {
        if (string_cmp(res->addresses->data[i]->data, address) != 0) {
            list->data[size++] = object_copy(res->addresses->data[i]);
        }
    }
    list_resize(list, size);
    list_set(res->addresses, list);
    list_free(list);
}
int account_connections_contains(const struct account_connections *res, const struct string_st *address) {
    if (res == NULL || string_is_null(address)) return 0;

    for (size_t i = 0; i < res->addresses->size; i++) {
        if (string_cmp(res->addresses->data[i]->data, address) == 0) return 1;
    }
    return 0;
}
