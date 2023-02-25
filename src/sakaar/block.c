#include "sakaar.h"
#include "crypto_base.h"

// Local Data Methods
void block_get(struct block_st *block, const struct string_st *hash, const struct integer_st *time) {
    if (block == NULL) return;
    if (string_is_null(hash) || integer_is_null(time)) return block_clear(block);

    if (main_config->_is_server) return block_load(block, hash, time);
    return block_request(block, hash, time);
}
void block_save_local(const struct block_st *block) {
    if (block == NULL) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

#ifdef WIN32
    string_set_str(path, ".data", 5);
    mkdir(path->data);
    string_set_str(path, ".data/block_active/", 19);
    mkdir(path->data);
#else
    string_set_str(path, ".data", 5);
    mkdir(path->data, 0777);
    string_set_str(path, ".data/block_active/", 19);
    mkdir(path->data, 0777);
#endif

    integer_get_str(block->time, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "_", 1);
    string_concat(path, tlv);
    string_concat(path, block->hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    block_get_tlv(block, tlv);
    file_write(path, tlv);

    string_free(tlv);
    string_free(path);
}
int block_exist(const struct string_st *hash, const struct integer_st *time) {
    if (string_is_null(hash) || integer_is_null(time)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/block_active/", 19);
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
void block_load(struct block_st *block, const struct string_st *hash, const struct integer_st *time) {
    if (block == NULL) return;
    block_clear(block);
    if (string_is_null(hash) || integer_is_null(time)) return;

    struct string_st *path = string_new();
    struct string_st *tlv = string_new();

    string_set_str(path, ".data/block_active/", 19);
    integer_get_str(time, tlv);
    string_concat(path, tlv);
    string_set_str(tlv, "_", 1);
    string_concat(path, tlv);
    string_concat(path, hash);
    string_set_str(tlv, ".skr", 4);
    string_concat(path, tlv);

    file_read(path, tlv);
    if (tlv->size != 0) block_set_tlv(block, tlv);

    string_free(tlv);
    string_free(path);
}
int block_remove(const struct string_st *hash, const struct integer_st *time) {
    if (string_is_null(hash) || integer_is_null(time)) return 0;

    struct string_st *path = string_new();
    struct string_st *path_end = string_new();

    string_set_str(path, ".data/block_active/", 19);
    integer_get_str(time, path_end);
    string_concat(path, path_end);
    string_set_str(path_end, "_", 1);
    string_concat(path, path_end);
    string_concat(path, hash);
    string_set_str(path_end, ".skr", 4);
    string_concat(path, path_end);

    int result = file_remove(path);

    string_free(path);
    string_free(path_end);
    return result;
}

// NetWork Methods
void block_request(struct block_st *res, const struct string_st *hash, const struct integer_st *time) {
    struct string_st *req = string_new();
    struct string_st *resp = string_new();
    block_clear(res);

    {
        struct string_st *tlv = string_new();
        string_get_tlv(hash, req);

        integer_get_tlv(time, tlv);
        string_concat(req, tlv);

        tlv_set_string(req, TLV_REQ_BLOCK, req);
        string_free(tlv);
    }
    config_network_get(req, resp);
    if (!string_is_null(resp)) block_set_tlv(res, resp);

    string_free(req);
    string_free(resp);
}
void block_response(struct string_st *res, const struct string_st *req) {
    if (tlv_get_tag(req->data) != TLV_REQ_BLOCK) return;
    string_clear(res);

    struct string_st *hash = string_new();
    struct integer_st *time = integer_new();
    struct block_st *block = block_new();

    {
        char *_data = tlv_get_value(req->data);
        struct string_st *_tlv = string_new();

        _data = tlv_get_next_tlv(_data, _tlv);
        string_set_tlv(hash, _tlv);

        tlv_get_next_tlv(_data, _tlv);
        integer_set_tlv(time, _tlv);

        string_free(_tlv);
    }
    if (block_exist(hash, time)) {
        block_get(block, hash, time);
        block_get_tlv(block, res);
    }

    string_free(hash);
    integer_free(time);
    block_free(block);
}

// Class Methods
struct currency_st *get_from_all_currency(struct list_st *all_currencies, struct string_st *cr);
struct wallet_st *get_from_all_wallets(struct list_st *all_wallets, struct string_st *cr, struct string_st *ad);
void make_block(struct block_st *res, const struct list_st *transactions, const struct string_st *smart_contract, const struct string_st *address_outside) {
    if (res == NULL) return;

    block_clear(res);
    integer_set_time(res->time);


    if (transactions != NULL) {
        struct list_st *all_currencies = list_new();
        struct list_st *all_wallets = list_new();
        struct string_st *hash = string_new();

        struct pre_transaction *tran = NULL;
        struct currency_st *currency = NULL;
        struct wallet_st *wallet1 = NULL;
        struct wallet_st *wallet2 = NULL;
        for (size_t i = 0; i < transactions->size; i++) {
            tran = transactions->data[i]->data;
            currency = get_from_all_currency(all_currencies, tran->currency);
            wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
            wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);

            if (currency == NULL || wallet1 == NULL || wallet2 == NULL) {
                list_free(all_currencies);
                list_free(all_wallets);
                string_free(hash);
                block_clear(res);
                return;
            }

            {
                list_add_new(res->transactions, TRANSACTION_TYPE);
                struct transaction_st *trans = res->transactions->data[res->transactions->size - 1]->data;

                transaction_set_data(trans, tran, wallet1->data->pre_hash, wallet1->data->pre_balance);
                transaction_get_hash(trans, hash, res->time);
                transaction_create_sign(trans, tran->private_key, currency, hash);
            }


            integer_sub(wallet1->data->pre_balance, wallet1->data->pre_balance, tran->balance);
            integer_add(wallet2->data->pre_balance, wallet2->data->pre_balance, tran->balance);
            if (!(string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 && wallet1->smart->owner->size != 0)) {
                integer_sub(wallet1->data->pre_balance, wallet1->data->pre_balance, tran->fee);
                integer_add(res->benefit, res->benefit, tran->fee);
            }
            string_concat(wallet1->data->pre_hash, hash);
            get_hash_code(integer_get_si(currency->hash_type))._code(wallet1->data->pre_hash, wallet1->data->pre_hash);

            string_concat(res->hash, hash);
            sha256_code._code(res->hash, res->hash);
        }

        list_free(all_currencies);
        list_free(all_wallets);
        string_free(hash);
    }
    string_concat(res->hash, smart_contract);
    sha256_code._code(res->hash, res->hash);
    string_concat(res->hash, address_outside);
    sha256_code._code(res->hash, res->hash);

    string_set(res->address_outside, address_outside);
    string_set(res->smart_contract, smart_contract);
}
void block_get_history(const struct block_st *block, int result, struct block_history *res) {
    if (res == NULL) return;
    if (block == NULL) return block_history_clear(res);

    list_set(res->transactions, block->transactions);
    string_set(res->address_outside, block->address_outside);
    string_set(res->hash, block->hash);
    string_set(res->smart_contract, block->smart_contract);
    integer_set(res->benefit, block->benefit);
    integer_set(res->time, block->time);
    integer_set_ui(res->result, result);
}
void block_get_optimal_unchecked(struct block_st *res) {
    if (res == NULL) return;

    DIR *d;
    int ok = 0;

    d = opendir(".data/block_active/");
    if (d) {
        struct dirent *dir;
        struct block_st *block = block_new();
        struct string_st *dir_name = string_new();
        struct string_st *path = string_new();
        struct string_st *tlv = string_new();

        while ((dir = readdir(d)) != NULL) {
            string_set_str(dir_name, dir->d_name, strlen(dir->d_name));
            if (memcmp(dir_name->data + dir_name->size - 4, ".skr", 4) == 0) {
                string_set_str(path, ".data/block_active/", 19);
                string_concat(path, dir_name);
                file_read(path, tlv);
                if (tlv->size != 0) {
                    block_set_tlv(block, tlv);
                    if (integer_get_ui(block->voted) == 0) {
                        if (!ok || integer_cmp(block->benefit, res->benefit) >= 0) {
                            block_set(res, block);
                            ok = 1;
                        }
                        if (integer_cmp(block->benefit, res->benefit) == 0 || integer_cmp(block->time, res->time) < 0) {
                            block_set(res, block);
                        }
                    }
                }
            }
        }

        closedir(d);

        string_free(dir_name);
        string_free(path);
        string_free(tlv);
        block_free(block);
    }
    if (!ok) {
        block_clear(res);
    }
}
void block_get_optimal_checked(struct block_st *res, int *result) {
    if (res == NULL) return;

    DIR *d;
    int ok = 0;

    d = opendir(".data/block_active/");
    if (d) {
        struct dirent *dir;
        struct block_st *block = block_new();
        struct string_st *dir_name = string_new();
        struct string_st *path = string_new();
        struct string_st *tlv = string_new();
        struct integer_st *temp = integer_new();
        int blocK_result;

        while ((dir = readdir(d)) != NULL) {
            string_set_str(dir_name, dir->d_name, strlen(dir->d_name));
            if (memcmp(dir_name->data + dir_name->size - 4, ".skr", 4) == 0) {
                string_set_str(path, ".data/block_active/", 19);
                string_concat(path, dir_name);
                file_read(path, tlv);
                if (tlv->size != 0) {
                    block_set_tlv(block, tlv);
                    blocK_result = 2;
                    {
                        integer_set(temp, block->result_pros);
                        integer_ls(temp, temp, 1);
                        if (integer_cmp(temp, main_config->full_freeze) >= 0) {
                            blocK_result = 1;
                        }
                    }
                    {
                        integer_set(temp, block->result_cons);
                        integer_ls(temp, temp, 1);
                        if (integer_cmp(temp, main_config->full_freeze) > 0) {
                            blocK_result = 0;
                        }
                    }
                    if (blocK_result != 2) {
                        if (!ok || integer_cmp(block->benefit, res->benefit) >= 0) {
                            block_set(res, block);
                            *result = blocK_result;
                            ok = 1;
                        }
                        if (integer_cmp(block->benefit, res->benefit) == 0 || integer_cmp(block->time, res->time) < 0) {
                            block_set(res, block);
                            *result = blocK_result;
                        }
                    }
                }
            }
        }

        closedir(d);

        string_free(dir_name);
        string_free(path);
        string_free(tlv);
        block_free(block);
        integer_free(temp);
    }
    if (!ok) {
        block_clear(res);
    }
}
