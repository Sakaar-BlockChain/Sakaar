#include "sakaar.h"
#include "crypto_base.h"

struct block_result_data {
    struct integer_st *time;
    struct block_st *block;
    struct integer_st *result;
    struct string_st *address;
    struct string_st *signature;
};

struct block_result_data *block_result_data_new() {
    struct block_result_data *res = skr_malloc(sizeof(struct block_result_data));
    res->time = integer_new();
    res->block = block_new();
    res->result = integer_new();
    res->address = string_new();
    res->signature = string_new();
    return res;
}
void block_result_data_clear(struct block_result_data *res) {
    integer_clear(res->time);
    block_clear(res->block);
    integer_clear(res->result);
    string_clear(res->address);
    string_clear(res->signature);
}
void block_result_data_free(struct block_result_data *res) {
    integer_free(res->time);
    block_free(res->block);
    integer_free(res->result);
    string_free(res->address);
    string_free(res->signature);
    skr_free(res);
}

void block_result_data_set_tlv(struct block_result_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    block_result_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_GEN_BLOCK_RESULT) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    block_set_tlv(data->block, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->result, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void block_result_data_get_tlv(const struct block_result_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    block_get_tlv(data->block, _tlv);
    string_concat(tlv, _tlv);

    integer_get_tlv(data->result, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_GEN_BLOCK_RESULT, tlv);
    string_free(_tlv);
}
void block_result_data_get_signature_str(const struct block_result_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    block_get_tlv(data->block, _tlv);
    string_concat(str, _tlv);

    integer_get_tlv(data->result, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_generation_block_result(const struct generation *res) {
    int result = 0;

    struct block_result_data *data = block_result_data_new();
    struct activated_accounts *activated = activated_accounts_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();
    struct block_st *block = block_new();

    block_result_data_set_tlv(data, res->data);


    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    {
        block_result_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    if (block_is_null(data->block)) goto end;

    activated_accounts_get(activated);
    if (block_exist(data->block->hash, data->block->time)) {
        block_get(block, data->block->hash, data->block->time);
        struct list_st *sub_list = NULL;
        int _sub_res = 0;
        for (size_t i = 0; i < activated->addresses->size; i++) {
            sub_list = activated->addresses->data[i]->data;
            if (string_cmp(sub_list->data[0]->data, data->address) == 0) _sub_res = 1;
        }
        for (size_t i = 0; i < block->nodes_done->size; i++) {
            if (string_cmp(block->nodes_done->data[i]->data, data->address) == 0) _sub_res = 0;
        }
        if (_sub_res == 0) goto end;
    }
    if (block_history_exist(data->block->hash, data->block->time)) goto end;

    result = 1;

    end:

    block_result_data_free(data);
    activated_accounts_free(activated);
    account_free(account);
    string_free(hash);
    block_free(block);
    return result;
}
int cre_generation_block_result(const struct block_st *block, int result, const struct account_st *account, const struct string_st *key) {
    if (block_is_null(block) || string_is_null(key) || account_is_null(account)) return 0;
    struct generation *res = generation_new();
    {
        struct block_result_data *data = block_result_data_new();
        struct string_st *hash = string_new();

        integer_set(data->time, res->time);
        block_set(data->block, block);
        integer_set_ui(data->result, result);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        {
            block_result_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        block_result_data_get_tlv(data, res->data);
        block_result_data_free(data);
        string_free(hash);
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
void mk_generation_block_result(const struct generation *res) {
    struct block_result_data *data = block_result_data_new();
    block_result_data_set_tlv(data, res->data);

    {
        struct integer_st *freeze = integer_new();
        {
            struct list_st *sub_list = NULL;
            struct activated_accounts *activated = activated_accounts_new();
            activated_accounts_get(activated);
            for (size_t i = 0; i < activated->addresses->size; i++) {
                sub_list = activated->addresses->data[i]->data;
                if (string_cmp(sub_list->data[0]->data, data->address) == 0)
                    integer_set(freeze, sub_list->data[1]->data);
            }
            activated_accounts_free(activated);
        }

        struct block_st *block = block_new();
        if (block_exist(data->block->hash, data->block->time)) {
            block_get(block, data->block->hash, data->block->time);
        } else {
            block_set(block, data->block);
            list_clear(block->nodes_done);
            integer_clear(block->result_pros);
            integer_clear(block->result_cons);
            integer_clear(block->voted);
        }
        {
            list_add_new(block->nodes_done, STRING_TYPE);
            string_set(block->nodes_done->data[block->nodes_done->size - 1]->data, data->address);
        }

        if (string_cmp(data->address, main_config->account->address) == 0) {
            integer_set_ui(block->voted, 1);
        }
        if (integer_get_ui(data->result)) {
            integer_add(block->result_pros, block->result_pros, freeze);
        } else {
            integer_add(block->result_cons, block->result_cons, freeze);
        }
        block_save_local(block);

        block_free(block);
        integer_free(freeze);
    }

    block_result_data_free(data);
}

