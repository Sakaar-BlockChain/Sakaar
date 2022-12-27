#include "sakaar.h"
#include <sys/stat.h>
#include <dirent.h>

struct object_type block_history_type = {BLOCK_HISTORY_OP, METHOD_GET_TLV &block_history_get_tlv,
                                         METHOD_SET_TLV &block_history_set_tlv};


struct block_history *block_history_new() {
    struct block_history *res = skr_malloc(sizeof(struct block_history));
    res->transactions = list_new();
    res->address_outside = string_new();
    res->hash = string_new();
    res->smart_contract = string_new();
    res->benefit = integer_new();
    res->time = integer_new();
    res->result = integer_new();
    return res;
}
void block_history_set(struct block_history *res, const struct block_history *a) {
    if (a == NULL) return block_history_clear(res);

    list_set(res->transactions, a->transactions);
    string_set(res->address_outside, a->address_outside);
    string_set(res->hash, a->hash);
    string_set(res->smart_contract, a->smart_contract);
    integer_set(res->benefit, a->benefit);
    integer_set(res->time, a->time);
    integer_set(res->result, a->result);
}
void block_history_clear(struct block_history *res) {
    list_clear(res->transactions);
    string_clear(res->address_outside);
    string_clear(res->hash);
    string_clear(res->smart_contract);
    integer_clear(res->benefit);
    integer_clear(res->time);
    integer_clear(res->result);
}
void block_history_free(struct block_history *res) {
    list_free(res->transactions);
    string_free(res->address_outside);
    string_free(res->hash);
    string_free(res->smart_contract);
    integer_free(res->benefit);
    integer_free(res->time);
    integer_free(res->result);
    skr_free(res);
}

void history_get(struct list_st *list, const struct integer_st *time_from, const struct integer_st *time_to) {
    if (list == NULL) return;
    if (!main_config->_is_server) return history_request(list, time_from, time_to);
    list_clear(list);

    DIR *d;
    d = opendir(".data/history/");
    if (d) {
        struct dirent *dir;
        struct string_st *path = string_new();
        struct integer_st *_time = integer_new();
        size_t pos;
        size_t size;

        while ((dir = readdir(d)) != NULL) {
            size = strlen(dir->d_name);
            if (memcmp(dir->d_name + size - 4, ".skr", 4) == 0) {
                for (pos = 0; pos < size; pos++)if (dir->d_name[pos] == '_')break;
                _integer_set_str(_time, dir->d_name, pos);
                if ((integer_is_null(time_from) || integer_cmp(_time, time_from) > 0)
                    && (integer_is_null(time_to) || integer_cmp(_time, time_to) <= 0)) {
                    list_add_new(list, LIST_TYPE);
                    struct list_st *list_obj = list->data[list->size - 1]->data;
                    {
                        list_add_new(list_obj, INTEGER_TYPE);
                        integer_set(list_obj->data[list_obj->size - 1]->data, _time);
                    }
                    {
                        list_add_new(list_obj, STRING_TYPE);
                        string_set_str(list_obj->data[list_obj->size - 1]->data, dir->d_name + pos + 1, size - pos - 5);
                    }
                }
            }
        }

        closedir(d);

        string_free(path);
        integer_free(_time);
    }
}
void block_history_get(struct block_history *block, const struct string_st *hash, const struct integer_st *time) {
    if (block == NULL) return;
    if (string_is_null(hash) || integer_is_null(time)) return block_history_clear(block);

    if (main_config->_is_server) return block_history_load(block, hash, time);
    return block_history_request(block, hash, time);
}
void block_history_set_tlv(struct block_history *res, const struct string_st *tlv) {
    if (res == NULL) return;
    block_history_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_BLOCK_HISTORY) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    data = tlv_get_next_tlv(data, _tlv);
    list_set_tlv_self(res->transactions, _tlv, TRANSACTION_TYPE);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address_outside, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->hash, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->smart_contract, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->benefit, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->time, _tlv);

    tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->result, _tlv);

    string_free(_tlv);
}
void block_history_get_tlv(const struct block_history *block, struct string_st *res) {
    if (res == NULL) return;
    if (block == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    list_get_tlv(block->transactions, res);

    string_get_tlv(block->address_outside, tlv);
    string_concat(res, tlv);

    string_get_tlv(block->hash, tlv);
    string_concat(res, tlv);

    string_get_tlv(block->smart_contract, tlv);
    string_concat(res, tlv);

    integer_get_tlv(block->benefit, tlv);
    string_concat(res, tlv);

    integer_get_tlv(block->time, tlv);
    string_concat(res, tlv);

    integer_get_tlv(block->result, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_BLOCK_HISTORY, res);
    string_free(tlv);
}


void block_history_save_local(const struct block_history *block) {
    if (block == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/history/", 14);
    mkdir(path->data, 0777);

    integer_get_str(block->time, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "_", 1);
    string_concat(path, tlv);
    string_concat(path, block->hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    block_history_get_tlv(block, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int block_history_exist(const struct string_st *hash, const struct integer_st *time) {
    if (string_is_null(hash) || integer_is_null(time)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/history/", 14);
    integer_get_str(time, path_end);
    string_concat(path, path_end);
    string_set_str(path_end, "_", 1);
    string_concat(path, path_end);
    string_concat(path, hash);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    int result = file_exist(path);

    string_free(path);
    string_free(path_end);
    return result;
}
void block_history_load(struct block_history *block, const struct string_st *hash, const struct integer_st *time) {
    if (block == NULL) return;
    block_history_clear(block);
    if (string_is_null(hash) || integer_is_null(time)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/history/", 14);
    integer_get_str(time, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "_", 1);
    string_concat(path, tlv);
    string_concat(path, hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) block_history_set_tlv(block, tlv);

    string_free(tlv);
    string_free(path);
}

void block_history_request(struct block_history *res, const struct string_st *hash, const struct integer_st *time) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    block_history_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(hash, req);

        integer_get_tlv(time, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_BLOCK_HISTORY, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) block_history_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void block_history_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_BLOCK_HISTORY) return;
    string_clear(res);

    struct string_st *hash = string_new();
    struct integer_st *time = integer_new();
    struct block_history *block = block_history_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(hash, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        integer_set_tlv(time, _tlv);

        string_free(_tlv);
    }
    if (block_history_exist(hash, time)) {
        block_history_get(block, hash, time);
        block_history_get_tlv(block, res);
    }

    string_free(hash);
    integer_free(time);
    block_history_free(block);
}

void history_request(struct list_st *res, const struct integer_st *time_from, const struct integer_st *time_to) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    list_clear(res);

    {
        struct string_st *tlv = string_new();
        integer_get_tlv(time_from, req);

        integer_get_tlv(time_to, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_BLOCK_HISTORY_LIST, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) {
        list_set_tlv_self(res, resp, LIST_TYPE);
        {
            struct list_st *sub_list;
            for (size_t i = 0; i < res->size; i++) {
                sub_list = res->data[i]->data;
                object_set_tlv_self(sub_list->data[0], INTEGER_TYPE);
                object_set_tlv_self(sub_list->data[1], STRING_TYPE);
            }
        }
    }

    string_free(req);
    string_free(resp);
}
void history_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_BLOCK_HISTORY_LIST) return;
    string_clear(res);

    struct integer_st *time_from = integer_new();
    struct integer_st *time_to = integer_new();
    struct list_st *list_history = list_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        tlv_get_next_tlv(_data, _tlv);
        integer_set_tlv(time_from, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        integer_set_tlv(time_to, _tlv);

        string_free(_tlv);
    }
    history_get(list_history, time_from, time_to);
    list_get_tlv(list_history, res);

    integer_free(time_from);
    integer_free(time_to);
    list_free(list_history);
}
