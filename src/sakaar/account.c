#include "sakaar.h"

// Local Data Methods
void account_get(struct account_st *account, const struct string_st *address) {
    if (account == NULL) return;
    if (string_is_null(address)) return account_clear(account);

    if (main_config->_is_server) return account_load(account, address);
    return account_request(account, address);
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

// NetWork Methods
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
