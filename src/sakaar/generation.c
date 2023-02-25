#include "sakaar.h"
#include "crypto_base.h"
#ifdef WIN32
#include <stdio.h>
#endif

int che_generation_wallet_create(const struct generation *res);
int che_generation_account_username(const struct generation *res);
int che_generation_block_create(const struct generation *res);
int che_generation_block_result(const struct generation *res);

int generation_check(const struct generation *res) {
    if (!generate_check_hash(res)) return 0;
    switch (tlv_get_tag(res->data->data)) {
        case TLV_SMRT_WALLET_CREATE:
            return che_generation_wallet_create(res);
        case TLV_GEN_ACCOUNT_USERNAME:
            return che_generation_account_username(res);
        case TLV_GEN_BLOCK_CREATE:
            return che_generation_block_create(res);
        case TLV_GEN_BLOCK_RESULT:
            return che_generation_block_result(res);
    }
    return 0;
}

void mk_generation_wallet_create(const struct generation *res);
void mk_generation_account_username(const struct generation *res);
void mk_generation_block_create(const struct generation *res);
void mk_generation_block_result(const struct generation *res);

void generation_make(const struct generation *res) {
    switch (tlv_get_tag(res->data->data)) {
        case TLV_SMRT_WALLET_CREATE:
            return mk_generation_wallet_create(res);
        case TLV_GEN_ACCOUNT_USERNAME:
            return mk_generation_account_username(res);
        case TLV_GEN_BLOCK_CREATE:
            return mk_generation_block_create(res);
        case TLV_GEN_BLOCK_RESULT:
            return mk_generation_block_result(res);
    }
}

// Local Files Methods
void file_write(const struct string_st *path, const struct string_st *data) {
    if (path == NULL || data == NULL) return;

    FILE *output = fopen(path->data, "w");
    if (output == NULL) return;
    fwrite(data->data, 1, data->size, output);
    fclose(output);
}
void file_read(const struct string_st *path, struct string_st *data) {
    if (path == NULL || data == NULL) return;

    FILE *input = fopen(path->data, "r");
    if (input == NULL) {
        string_clear(data);
        return;
    }
    fseek(input, 0, SEEK_END);
    size_t size = ftell(input);
    string_resize(data, size);
    fseek(input, 0, SEEK_SET);
    fread(data->data, 1, data->size, input);
    fclose(input);
}
int file_exist(const struct string_st *path) {
    if (path == NULL) return 0;

    FILE *input = fopen(path->data, "r");
    if (input == NULL) return 0;
    fclose(input);
    return 1;
}
int file_remove(const struct string_st *path) {
    return path == NULL || !remove(path->data);
}

// Local Data Methods
void history_generation_get(struct list_st *list, const struct integer_st *time_from, const struct integer_st *time_to) {
    if (list == NULL) return;
    list_clear(list);

    DIR *d;
    d = opendir(".data/generation/");
    if (d) {
        struct dirent *dir;
        struct string_st *path = string_new();
        struct integer_st *_time = integer_new();
        struct string_st *_hash = string_new();
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
        string_free(_hash);
    }
}
void generation_get(struct generation *res, const struct string_st *hash, const struct integer_st *time) {
    if (res == NULL) return;
    if (string_is_null(hash) || integer_is_null(time)) return generation_clear(res);

    if (main_config->_is_server) return generation_load(res, hash, time);
    return generation_request(res, hash, time);
}
void generation_save_local(const struct generation *gen) {
    if (gen == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

#ifdef WIN32
    string_set_str(path, ".data", 5);
    mkdir(path->data);
    string_set_str(path, ".data/generation/", 17);
    mkdir(path->data);
#else
    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/generation/", 17);
    mkdir(path->data, 0777);
#endif

    integer_get_str(gen->time, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "_", 1);
    string_concat(path, tlv);
    string_concat(path, gen->hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    generation_get_tlv(gen, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int generation_exist(const struct string_st *hash, const struct integer_st *time) {
    if (string_is_null(hash) || integer_is_null(time)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/generation/", 17);
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
void generation_load(struct generation *gen, const struct string_st *hash, const struct integer_st *time) {
    if (gen == NULL) return;
    generation_clear(gen);
    if (string_is_null(hash) || integer_is_null(time)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/generation/", 17);
    string_concat(path, hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) generation_set_tlv(gen, tlv);

    string_free(tlv);
    string_free(path);
}

// NetWork Methods
void generation_request(struct generation *res, const struct string_st *hash, const struct integer_st *time) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    generation_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(hash, req);

        integer_get_tlv(time, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_GENERATION, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) generation_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void generation_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_GENERATION) return;
    string_clear(res);

    struct string_st *hash = string_new();
    struct integer_st *time = integer_new();
    struct generation *gen = generation_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(hash, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        integer_set_tlv(time, _tlv);

        string_free(_tlv);
    }
    if (generation_exist(hash, time)) {
        generation_get(gen, hash, time);
        generation_get_tlv(gen, res);
    }

    string_free(hash);
    integer_free(time);
    generation_free(gen);
}

void history_generation_request(struct list_st *res, const struct integer_st *time_from, const struct integer_st *time_to) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    list_clear(res);

    {
        struct string_st *tlv = string_new();
        integer_get_tlv(time_from, req);

        integer_get_tlv(time_to, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_GENERATION_LIST, req);
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
void history_generation_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_GENERATION_LIST) return;
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
    history_generation_get(list_history, time_from, time_to);
    list_get_tlv(list_history, res);

    integer_free(time_from);
    integer_free(time_to);
    list_free(list_history);
}

// Class Methods
void generate_make_hash(struct generation *res) {
    if (res == NULL) return;

    struct string_st *hash = string_new();

    integer_get_str(res->time, hash);
    string_concat(hash, res->data);

    sha256_code._code(res->hash, hash);
    string_free(hash);
}
int generate_check_hash(const struct generation *res) {
    if (res == NULL) return 0;

    struct string_st *hash = string_new();

    integer_get_str(res->time, hash);
    string_concat(hash, res->data);

    sha256_code._code(hash, hash);
    int result = (string_cmp(hash, res->hash) == 0);
    string_free(hash);
    return result;
}
int generation_work(const struct generation *res) {
    if (!main_config->_is_server) return 1;
    if (!generation_check(res)) return 0;
    generation_make(res);
    unsigned tag = tlv_get_tag(res->data->data);
    if (tag == TLV_GEN_BLOCK_CREATE || tag == TLV_GEN_BLOCK_RESULT)
        generation_save_local(res);
    return 1;
}
