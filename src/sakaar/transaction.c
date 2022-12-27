#include "sakaar.h"
#include "crypto_base.h"

struct object_type transaction_type = {TRANSACTION_OP, METHOD_GET_TLV &transaction_get_tlv,
                                       METHOD_SET_TLV &transaction_set_tlv};


struct transaction_st *transaction_new() {
    struct transaction_st *res = skr_malloc(sizeof(struct transaction_st));
    res->address_from = string_new();
    res->address_to = string_new();
    res->currency = string_new();

    res->balance = integer_new();
    res->fee = integer_new();

    res->balance_from = integer_new();
    res->hash_from = string_new();

    res->signature = string_new();
    return res;
}
void transaction_set(struct transaction_st *res, const struct transaction_st *a) {
    if (a == NULL) return transaction_clear(res);

    string_set(res->address_from, a->address_from);
    string_set(res->address_to, a->address_to);
    string_set(res->currency, a->currency);

    integer_set(res->balance, a->balance);
    integer_set(res->fee, a->fee);

    integer_set(res->balance_from, a->balance_from);
    string_set(res->hash_from, a->hash_from);

    string_set(res->signature, a->signature);
}
void transaction_clear(struct transaction_st *res) {
    string_clear(res->address_from);
    string_clear(res->address_to);
    string_clear(res->currency);

    integer_clear(res->balance);
    integer_clear(res->fee);

    integer_clear(res->balance_from);
    string_clear(res->hash_from);

    string_clear(res->signature);
}
void transaction_free(struct transaction_st *res) {
    string_free(res->address_from);
    string_free(res->address_to);
    string_free(res->currency);

    integer_free(res->balance);
    integer_free(res->fee);

    integer_free(res->balance_from);
    string_free(res->hash_from);

    string_free(res->signature);
    skr_free(res);
}

void transaction_set_data(struct transaction_st *res, struct pre_transaction *pre_tran, struct string_st *hash_from, struct integer_st *balance_from) {
    if (res == NULL) return;
    if (pre_tran == NULL || hash_from == NULL || balance_from == NULL) return transaction_clear(res);

    string_set(res->address_from, pre_tran->address_from);
    string_set(res->address_to, pre_tran->address_to);
    string_set(res->currency, pre_tran->currency);

    integer_set(res->balance, pre_tran->balance);
    integer_set(res->fee, pre_tran->fee);

    integer_set(res->balance_from, balance_from);
    string_set(res->hash_from, hash_from);
}
void transaction_get_hash(const struct transaction_st *res, struct string_st *hash, const struct integer_st *time) {
    if (hash == NULL) return;
    if (time == NULL || res == NULL) return string_free(hash);

    struct string_st *temp = string_new();
    string_concat(temp, res->address_from);
    string_concat(temp, res->address_to);
    string_concat(temp, res->currency);

    {
        integer_get_tlv(res->balance, hash);
        string_concat(temp, hash);
    }
    {
        integer_get_tlv(time, hash);
        string_concat(temp, hash);
    }

    {
        integer_get_tlv(res->balance_from, hash);
        string_concat(temp, hash);
    }
    string_concat(temp, res->hash_from);

    sha256_code._code(hash, temp);
    string_free(temp);
}
void transaction_create_sign(const struct transaction_st *res, const struct string_st *key, const struct currency_st *currency, struct string_st *hash) {
    if (hash == NULL) return;
    if (res == NULL || string_is_null(key)) return string_clear(hash);

    return get_crypto_base(integer_get_ui(currency->crypto_base))._create_sign(res->signature, key, hash);
}

void transaction_set_tlv(struct transaction_st *res, const struct string_st *tlv) {
    if (res == NULL) return;
    transaction_clear(res);
    if (string_is_null(tlv) || tlv_get_tag(tlv->data) != TLV_TRANSACTION) return;

    char *data = tlv_get_value(tlv->data);
    struct string_st *_tlv = string_new();


    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address_from, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->address_to, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->currency, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->balance, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->fee, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    integer_set_tlv(res->balance_from, _tlv);

    data = tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->hash_from, _tlv);

    tlv_get_next_tlv(data, _tlv);
    string_set_tlv(res->signature, _tlv);

    string_free(_tlv);
}
void transaction_get_tlv(const struct transaction_st *account_conn, struct string_st *res) {
    if (res == NULL) return;
    if (account_conn == NULL) return string_clear(res);

    struct string_st *tlv = string_new();
    string_get_tlv(account_conn->address_from, res);

    string_get_tlv(account_conn->address_to, tlv);
    string_concat(res, tlv);

    string_get_tlv(account_conn->currency, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->balance, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->fee, tlv);
    string_concat(res, tlv);

    integer_get_tlv(account_conn->balance_from, tlv);
    string_concat(res, tlv);

    string_get_tlv(account_conn->hash_from, tlv);
    string_concat(res, tlv);

    string_get_tlv(account_conn->signature, tlv);
    string_concat(res, tlv);

    tlv_set_string(res, TLV_TRANSACTION, res);
    string_free(tlv);
}
