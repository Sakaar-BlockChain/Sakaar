#include "sakaar.h"

// Local Data Methods
void activated_accounts_get(struct activated_accounts *activated_acc) {
    if (activated_acc == NULL) return;
    activated_accounts_clear(activated_acc);

    if (main_config->_is_server) return activated_accounts_load(activated_acc);
    return activated_accounts_request(activated_acc);
}
void activated_accounts_save_local(const struct activated_accounts *activated) {
    if (activated == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

#ifdef WIN32
    string_set_str(path, ".data", 5);
    mkdir(path->data);
#else
    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
#endif

    string_set_str(path, ".data/activated.skr", 19);

    activated_accounts_get_tlv(activated, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
void activated_accounts_load(struct activated_accounts *activated) {
    if (activated == NULL) return;
    activated_accounts_clear(activated);

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();
    string_set_str(path, ".data/activated.skr", 19);

    file_read(path, tlv);
    if (tlv->size != 0) activated_accounts_set_tlv(activated, tlv);

    string_free(tlv);
    string_free(path);
}

// NetWork Methods
void activated_accounts_request(struct activated_accounts *res) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    activated_accounts_clear(res);

    {
        string_clear(req);
        tlv_set_string(req, TLV_REQ_ACTIVE_ACC, req);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) activated_accounts_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void activated_accounts_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_ACTIVE_ACC) return;
    string_clear(res);

    struct activated_accounts *active_acc = activated_accounts_new();

    activated_accounts_get(active_acc);
    activated_accounts_get_tlv(active_acc, res);

    activated_accounts_free(active_acc);
}

// Class Methods
void activated_accounts_add(struct activated_accounts *res, const struct string_st *address, const struct integer_st *freeze) {
    if (res == NULL || string_is_null(address) || integer_is_null(freeze)) return;

    list_add_new(res->addresses, LIST_TYPE);
    struct list_st *list = res->addresses->data[res->addresses->size - 1]->data;

    list_add_new(list, STRING_TYPE);
    list_add_new(list, INTEGER_TYPE);
    string_set(list->data[0]->data, address);
    integer_set(list->data[1]->data, freeze);
}
void activated_accounts_remove(struct activated_accounts *res, const struct string_st *address) {
    if (res == NULL || address == NULL) return;

    struct list_st *list = list_new();
    list_resize(list, res->addresses->size);
    size_t size = 0;
    for (size_t i = 0; i < res->addresses->size; i++) {
        if (string_cmp(((struct list_st *) res->addresses->data[i]->data)->data[0]->data, address) != 0) {
            list->data[size++] = object_copy(res->addresses->data[i]);
        }
    }
    list_resize(list, size);
    list_set(res->addresses, list);
    list_free(list);
}
int activated_accounts_contains(const struct activated_accounts *res, const struct string_st *address) {
    if (res == NULL || address == NULL) return 0;

    for (size_t i = 0; i < res->addresses->size; i++) {
        if (string_cmp(((struct list_st *) res->addresses->data[i]->data)->data[0]->data, address) == 0) return 1;
    }
    return 0;
}
void activated_accounts_freeze(const struct activated_accounts *res, struct integer_st *freeze) {
    if (freeze == NULL) return;
    integer_clear(freeze);
    if (res == NULL) return;

    for (size_t i = 0; i < res->addresses->size; i++) {
        integer_add(freeze, freeze, ((struct list_st *) res->addresses->data[i]->data)->data[1]->data);
    }
}
