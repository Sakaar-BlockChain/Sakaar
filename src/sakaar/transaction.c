#include "sakaar.h"
#include "crypto_base.h"

// Class Methods
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
