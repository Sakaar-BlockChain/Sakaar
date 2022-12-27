#include "sakaar.h"
#include <sys/stat.h>

struct object_type account_conn_type = {ACCOUNT_CONN_OP, METHOD_GET_TLV &account_connections_get_tlv,
                                        METHOD_SET_TLV &account_connections_set_tlv};


struct account_connections *account_connections_new() {
    struct account_connections *res = skr_malloc(sizeof(struct account_connections));
    res->address = string_new();
    res->currency = string_new();
    res->addresses = list_new();
    return res;
}
void account_connections_set(struct account_connections *res, const struct account_connections *a) {
    if (a == NULL) return account_connections_clear(res);

    string_set(res->address, a->address);
    string_set(res->currency, a->currency);
    list_set(res->addresses, a->addresses);
}
void account_connections_clear(struct account_connections *res) {
    string_clear(res->address);
    string_clear(res->currency);
    list_clear(res->addresses);
}
void account_connections_free(struct account_connections *res) {
    string_free(res->address);
    string_free(res->currency);
    list_free(res->addresses);
    skr_free(res);
}

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

void account_connections_get(struct account_connections *account_conn, const struct string_st *currency, const struct string_st *address) {
    if (account_conn == NULL) return;
    if (string_is_null(currency) || string_is_null(address)) return account_connections_clear(account_conn);

    if (main_config->_is_server) return account_connections_load(account_conn, currency, address);
    return account_connections_request(account_conn, currency, address);
}
void account_connections_set_tlv(struct account_connections *res, const struct string_st *tlv) {
    if (res == NULL) return;
    account_connections_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_ACCOUNT_CONN) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->currency, _tlv);

    tlv_get_next_tlv(data, _tlv);
    list_set_tlv_self(res->addresses, _tlv, STRING_TYPE);

    string_free(_tlv);
}
void account_connections_get_tlv(const struct account_connections *account_conn, struct string_st *res) {
    if (res == NULL) return;
    if (account_conn == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    string_get_tlv(account_conn->address, res);

    string_get_tlv(account_conn->currency, tlv);
    string_concat(res, tlv);

    list_get_tlv(account_conn->addresses, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_ACCOUNT_CONN, res);
    string_free(tlv);
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
