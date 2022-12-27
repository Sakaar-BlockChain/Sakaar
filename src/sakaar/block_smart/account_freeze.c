#include "sakaar.h"
#include "crypto_base.h"

struct account_freeze_data {
    struct integer_st *time;
    struct integer_st *balance;
    struct string_st *w_address;
    struct string_st *address;
    struct string_st *signature;
};

struct account_freeze_data *account_freeze_data_new() {
    struct account_freeze_data *res = skr_malloc(sizeof(struct account_freeze_data));
    res->time = integer_new();
    res->balance = integer_new();
    res->w_address = string_new();
    res->address = string_new();
    res->signature = string_new();
    return res;
}
void account_freeze_data_clear(struct account_freeze_data *res) {
    integer_clear(res->time);
    integer_clear(res->balance);
    string_clear(res->address);
    string_clear(res->w_address);
    string_clear(res->signature);
}
void account_freeze_data_free(struct account_freeze_data *res) {
    integer_free(res->time);
    integer_free(res->balance);
    string_free(res->w_address);
    string_free(res->address);
    string_free(res->signature);
    skr_free(res);
}

void account_freeze_data_set_tlv(struct account_freeze_data *data, const struct string_st *tlv) {
    if (data == NULL) return;
    account_freeze_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_ACCOUNT_FREEZE) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->balance, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_address, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void account_freeze_data_get_tlv(const struct account_freeze_data *data, struct string_st *tlv) {
    if (tlv == NULL) return;
    if (data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    integer_get_tlv(data->balance, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_ACCOUNT_FREEZE, tlv);
    string_free(_tlv);
}
void account_freeze_data_get_signature_str(const struct account_freeze_data *data, struct string_st *str) {
    if (str == NULL) return;
    if (data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    integer_get_tlv(data->balance, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_account_freeze(const struct string_st *smart_contract) {
    int result = 0;

    struct account_freeze_data *data = account_freeze_data_new();
    struct account_st *account = account_new();
    struct wallet_st *wallet = wallet_new();
    struct string_st *hash = string_new();
    struct integer_st *temp = integer_new();
    account_freeze_data_set_tlv(data, smart_contract);


    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    string_set_str(hash, "SKR", 3);
    if (!wallet_data_exist(hash, data->w_address)) goto end;
    wallet_get(wallet, hash, data->w_address);

    {
        account_freeze_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    integer_add(temp, wallet->smart->freeze, data->balance);
    if (integer_cmp(temp, wallet->data->balance) > 0 || integer_is_neg(temp)) goto end;

    result = 1;

    end:

    account_freeze_data_free(data);
    account_free(account);
    wallet_free(wallet);
    integer_free(temp);
    string_free(hash);
    return result;
}
void cre_block_smart_account_freeze(const struct string_st *address, const struct integer_st *balance, const struct account_st *account, const struct string_st *key) {
    if (string_is_null(address) || integer_is_null(balance) || string_is_null(key) || account_is_null(account)) return;
    struct string_st *smart_contract = string_new();
    {
        struct account_freeze_data *data = account_freeze_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        integer_set(data->balance, balance);
        string_set(data->w_address, address);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);

        {
            account_freeze_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        account_freeze_data_get_tlv(data, smart_contract);
        account_freeze_data_free(data);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_account_freeze(const struct string_st *smart_contract) {
    struct account_freeze_data *data = account_freeze_data_new();
    account_freeze_data_set_tlv(data, smart_contract);

    {
        struct wallet_smart *smart = wallet_smart_new();
        struct string_st *skr_cur = string_new();

        string_set_str(skr_cur, "SKR", 3);
        wallet_smart_get(smart, skr_cur, data->w_address);
        integer_add(smart->freeze, smart->freeze, data->balance);
        wallet_smart_save_local(smart);

        wallet_smart_free(smart);
        string_free(skr_cur);
    }
    {
        struct account_st *account = account_new();
        account_get(account, data->address);
        integer_add(account->freeze, account->freeze, data->balance);
        account_save_local(account);
        account_free(account);
    }
    {
        struct activated_accounts *activated = activated_accounts_new();
        activated_accounts_get(activated);

        if (activated_accounts_contains(activated, data->address)) {
            struct list_st *sub_list = NULL;
            for (size_t i = 0; i < activated->addresses->size; i++) {
                sub_list = activated->addresses->data[i]->data;
                if (string_cmp(sub_list->data[0]->data, data->address) == 0) {
                    integer_add(sub_list->data[1]->data, sub_list->data[1]->data, data->balance);
                }
            }
            activated_accounts_save_local(activated);
        }
        activated_accounts_free(activated);
    }

    {
        integer_add(main_config->full_freeze, main_config->full_freeze, data->balance);
        if (!account_is_null(main_config->account) && string_cmp(main_config->account->address, data->address) == 0) {
            integer_add(main_config->account->freeze, main_config->account->freeze, data->balance);
        }
        config_save_local(main_config);
    }


    account_freeze_data_free(data);
}


int block_check(const struct block_st *block);
void block_save(const struct block_st *block, int result);
void my_cre_block_smart_account_freeze(const struct string_st *address, const struct integer_st *balance, const struct account_st *account, const struct string_st *key) {
    if (address == NULL || balance == NULL || key == NULL) return;
    struct string_st *smart_contract = string_new();
    {
        struct account_freeze_data *data = account_freeze_data_new();
        struct string_st *hash = string_new();

        integer_set_time(data->time);
        integer_set(data->balance, balance);
        string_set(data->w_address, address);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);

        {
            account_freeze_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        account_freeze_data_get_tlv(data, smart_contract);
        account_freeze_data_free(data);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);
    int result = block_check(block);
    printf("result : %d\n", result);
    if (result) block_save(block, 1);

    string_free(smart_contract);
    block_free(block);
}

