#include "sakaar.h"
#include "crypto_base.h"

struct wallet_disconnect_data{
    struct integer_st *time;

    struct string_st *address;
    struct string_st *w_address;
    struct string_st *currency;

    struct string_st *w_signature;
    struct string_st *signature;
};

struct wallet_disconnect_data *wallet_disconnect_data_new(){
    struct wallet_disconnect_data *res = skr_malloc(sizeof(struct wallet_disconnect_data));
    res->time = integer_new();

    res->address = string_new();
    res->w_address = string_new();
    res->currency = string_new();

    res->w_signature = string_new();
    res->signature = string_new();
    return res;
}
void wallet_disconnect_data_clear(struct wallet_disconnect_data *res){
    integer_clear(res->time);

    string_clear(res->address);
    string_clear(res->w_address);
    string_clear(res->currency);

    string_clear(res->w_signature);
    string_clear(res->signature);
}
void wallet_disconnect_data_free(struct wallet_disconnect_data *res){
    integer_free(res->time);

    string_free(res->address);
    string_free(res->w_address);
    string_free(res->currency);

    string_free(res->w_signature);
    string_free(res->signature);
    skr_free(res);
}

void wallet_disconnect_data_set_tlv(struct wallet_disconnect_data *data, const struct string_st *tlv){
    if(data == NULL) return;
    wallet_disconnect_data_clear(data);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_SMRT_WALLET_DISCONNECT) return;

    char *_data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();

    _data = tlv_get_next_tlv(_data, _tlv);
    integer_set_tlv(data->time, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_address, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->currency, _tlv);

    _data = tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->w_signature, _tlv);

    tlv_get_next_tlv(_data, _tlv);
    string_set_tlv(data->signature, _tlv);

    string_free(_tlv);
}
void wallet_disconnect_data_get_tlv(const struct wallet_disconnect_data *data, struct string_st *tlv){
    if(tlv == NULL) return;
    if(data == NULL) return string_clear(tlv);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, tlv);

    string_get_tlv(data->address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->currency, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->w_signature, _tlv);
    string_concat(tlv, _tlv);

    string_get_tlv(data->signature, _tlv);
    string_concat(tlv, _tlv);

    tlv_set_string(tlv, TLV_SMRT_WALLET_DISCONNECT, tlv);
    string_free(_tlv);
}
void wallet_disconnect_data_get_signature_str(const struct wallet_disconnect_data *data, struct string_st *str){
    if(str == NULL) return;
    if(data == NULL) return string_clear(str);

    struct string_st *_tlv = string_new();
    integer_get_tlv(data->time, str);

    string_get_tlv(data->address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->w_address, _tlv);
    string_concat(str, _tlv);

    string_get_tlv(data->currency, _tlv);
    string_concat(str, _tlv);

    string_free(_tlv);
}

int che_block_smart_wallet_disconnect(const struct string_st *smart_contract){
    int result = 0;

    struct wallet_disconnect_data *data = wallet_disconnect_data_new();
    struct account_connections *connections = account_connections_new();
    struct currency_st *currency = currency_new();
    struct wallet_st *wallet = wallet_new();
    struct account_st *account = account_new();
    struct string_st *hash = string_new();

    wallet_disconnect_data_set_tlv(data, smart_contract);

    if (!account_exist(data->address)) goto end;
    account_get(account, data->address);

    if (!currency_exist(data->currency)) goto end;
    currency_get(currency, data->currency);
    wallet_get(wallet, data->currency, data->w_address);

    {
        wallet_disconnect_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(account->crypto_base))._check_sign(data->signature, data->address, hash))
            goto end;
    }

    {
        wallet_disconnect_data_get_signature_str(data, hash);
        get_hash_code(integer_get_ui(currency->hash_type))._code(hash, hash);
        if (!get_crypto_base(integer_get_ui(currency->crypto_base))._check_sign(data->w_signature, data->w_address,
                                                                                hash))
            goto end;
    }

    if(string_cmp(wallet->smart->owner, data->address) != 0) goto end;

    account_connections_get(connections, data->currency, data->address);
    if(!account_connections_contains(connections, data->w_address)) goto end;

    result = 1;
    end:

    wallet_disconnect_data_free(data);
    currency_free(currency);
    wallet_free(wallet);
    account_connections_free(connections);
    account_free(account);
    string_free(hash);
    return result;
}
void cre_block_smart_wallet_disconnect(const struct string_st *currency, const struct string_st *address, const struct account_st *account, const struct string_st *key){
    if(string_is_null(currency) || string_is_null(address) || string_is_null(key) || account_is_null(account)) return;
    if(!currency_exist(currency)) return;
    struct string_st *smart_contract = string_new();
    {
        struct wallet_disconnect_data *data = wallet_disconnect_data_new();
        struct currency_st *Currency = currency_new();
        struct string_st *w_key = string_new();
        struct string_st *hash = string_new();
        currency_get(Currency, currency);

        {
            struct wallet_st *Wallet = wallet_new();
            wallet_get(Wallet, currency, address);
            wallet_smart_private(Wallet->smart, key, account, w_key);
            wallet_free(Wallet);
        }

        integer_set_time(data->time);
        get_crypto_base(integer_get_ui(account->crypto_base))._get_public(key, data->address);
        get_crypto_base(integer_get_ui(Currency->crypto_base))._get_public(w_key, data->w_address);
        string_set(data->currency, currency);
        {
            wallet_disconnect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(account->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(account->crypto_base))._create_sign(data->signature, key, hash);
        }

        {
            wallet_disconnect_data_get_signature_str(data, hash);
            get_hash_code(integer_get_ui(Currency->hash_type))._code(hash, hash);
            get_crypto_base(integer_get_ui(Currency->crypto_base))._create_sign(data->w_signature, w_key, hash);
        }


        wallet_disconnect_data_get_tlv(data, smart_contract);
        wallet_disconnect_data_free(data);
        currency_free(Currency);
        string_free(w_key);
        string_free(hash);
    }
    struct block_st *block = block_new();
    make_block(block, NULL, smart_contract, NULL);
    config_block_create(block);

    string_free(smart_contract);
    block_free(block);
}
void mk_block_smart_wallet_disconnect(const struct string_st *smart_contract){
    struct wallet_disconnect_data *data = wallet_disconnect_data_new();
    wallet_disconnect_data_set_tlv(data, smart_contract);

    {
        struct wallet_smart *smart = wallet_smart_new();
        wallet_smart_get(smart, data->currency, data->w_address);

        {
            struct activated_accounts *activated = activated_accounts_new();
            activated_accounts_get(activated);

            if(activated_accounts_contains(activated, data->address)){
                struct list_st *sub_list = NULL;
                for(size_t i=0; i<activated->addresses->size; i++){
                    sub_list = activated->addresses->data[i]->data;
                    if(string_cmp(sub_list->data[0]->data, data->address) == 0) {
                        integer_sub(sub_list->data[1]->data, sub_list->data[1]->data, smart->freeze);
                    }
                }

                activated_accounts_save_local(activated);
            }
            activated_accounts_free(activated);
        }
        {
            struct account_st *account = account_new();
            account_get(account, data->address);
            integer_sub(account->freeze, account->freeze, smart->freeze);
            account_save_local(account);
            account_free(account);
        }

        string_clear(smart->private_key);
        string_clear(smart->smart_private);
        string_clear(smart->smart_contract);
        string_clear(smart->name);
        string_clear(smart->owner);
        integer_clear(smart->freeze);

        wallet_smart_save_local(smart);

        wallet_smart_free(smart);
    }
    {
        struct account_connections *connections = account_connections_new();
        account_connections_get(connections, data->currency, data->address);

        account_connections_remove(connections, data->w_address);
        account_connections_save_local(connections);

        account_connections_free(connections);
    }

    wallet_disconnect_data_free(data);
}