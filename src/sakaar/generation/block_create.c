#include "sakaar.h"

struct block_create_data {
    struct block_st *block;
};

struct block_create_data *block_create_data_new() {
    struct block_create_data *res = skr_malloc(sizeof(struct block_create_data));
    res->block = block_new();
    return res;
}
void block_create_data_clear(struct block_create_data *res) {
    block_clear(res->block);
}
void block_create_data_free(struct block_create_data *res) {
    block_free(res->block);
    skr_free(res);
}

void block_create_data_set_tlv(struct block_create_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    block_create_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_GEN_BLOCK_CREATE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    tlv_get_next_tlv(_data, _tlv);
    block_set_tlv(data->block, _tlv);

    string_free(_tlv);
}
void block_create_data_get_tlv(const struct block_create_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    block_get_tlv(data->block, tlv);
    tlv_set_string(tlv, TLV_GEN_BLOCK_CREATE, tlv);
}

int che_generation_block_create(const struct generation *res) {
    int result = 0;

    struct block_create_data *data = block_create_data_new();
    block_create_data_set_tlv(data, res->data);

    {
        if (block_is_null(data->block)) goto end;
        if (block_exist(data->block->hash, data->block->time)) goto end;
        if (block_history_exist(data->block->hash, data->block->time)) goto end;
        result = 1;
    }

    end:

    block_create_data_free(data);
    return result;
}
int cre_generation_block_create(const struct block_st *block) {
    if (block_is_null(block)) return 0;
    struct generation *res = generation_new();
    {
        struct block_create_data *data = block_create_data_new();

        block_set(data->block, block);

        block_create_data_get_tlv(data, res->data);
        block_create_data_free(data);
    }
    generate_make_hash(res);
    int _result = generation_work(res);
    if (_result) {
        struct string_st *msg = string_new();
        generation_get_tlv(res, msg);
        config_network_send(msg);
        string_free(msg);
    }
    generation_free(res);
    return _result;
}
void mk_generation_block_create(const struct generation *res) {
    struct block_create_data *data = block_create_data_new();
    block_create_data_set_tlv(data, res->data);

    {
        struct block_st *block = block_new();
        block_set(block, data->block);
        list_clear(block->nodes_done);
        integer_clear(block->result_pros);
        integer_clear(block->result_cons);
        integer_clear(block->voted);

        block_save_local(block);
        block_free(block);
    }


    block_create_data_free(data);
}

