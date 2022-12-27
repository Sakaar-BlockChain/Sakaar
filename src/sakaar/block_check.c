#include "sakaar.h"
#include <sys/stat.h>
#include <dirent.h>
#include "crypto_base.h"

struct currency_st *get_from_all_currency(struct list_st *all_currencies, struct string_st *cr) {
    if (all_currencies == NULL || cr == NULL) return NULL;
    size_t id;
    struct currency_st *currency = NULL;
    for (id = 0; id < all_currencies->size; id++) {
        currency = all_currencies->data[id]->data;
        if (string_cmp(currency->name, cr) == 0) break;
    }
    if (id == all_currencies->size) {
        if (currency_exist(cr)) {
            list_add_new(all_currencies, CURRENCY_TYPE);
            currency_get(all_currencies->data[all_currencies->size - 1]->data, cr);
            currency = all_currencies->data[all_currencies->size - 1]->data;
        } else currency = NULL;
    }
    return currency;
}
struct wallet_st *get_from_all_wallets(struct list_st *all_wallets, struct string_st *cr, struct string_st *ad) {
    if (all_wallets == NULL || cr == NULL || ad == NULL) return NULL;
    size_t id;
    struct wallet_st *wallet = NULL;
    for (id = 0; id < all_wallets->size; id++) {
        wallet = all_wallets->data[id]->data;
        if (string_cmp(wallet->data->currency, cr) == 0 &&
            string_cmp(wallet->data->address, ad) == 0)
            break;
    }
    if (id == all_wallets->size) {
        list_add_new(all_wallets, WALLET_TYPE);
        wallet_get(all_wallets->data[all_wallets->size - 1]->data, cr, ad);
        wallet = all_wallets->data[all_wallets->size - 1]->data;
    }
    return wallet;
}

int block_check_hash(const struct block_st *block) {
    struct string_st *block_hash = string_new();
    struct string_st *hash = string_new();

    for (size_t i = 0; i < block->transactions->size; i++) {
        transaction_get_hash(block->transactions->data[i]->data, hash, block->time);
        string_concat(block_hash, hash);
        sha256_code._code(block_hash, block_hash);
    }

    string_concat(block_hash, block->smart_contract);
    sha256_code._code(block_hash, block_hash);
    string_concat(block_hash, block->address_outside);
    sha256_code._code(block_hash, block_hash);

    int result = (string_cmp(block_hash, block->hash) == 0);

    string_free(block_hash);
    string_free(hash);
    return result;
}
int block_history_check_hash(const struct block_history *block) {
    struct string_st *block_hash = string_new();
    struct string_st *hash = string_new();

    for (size_t i = 0; i < block->transactions->size; i++) {
        transaction_get_hash(block->transactions->data[i]->data, hash, block->time);
        string_concat(block_hash, hash);
        sha256_code._code(block_hash, block_hash);
    }

    string_concat(block_hash, block->smart_contract);
    sha256_code._code(block_hash, block_hash);
    string_concat(block_hash, block->address_outside);
    sha256_code._code(block_hash, block_hash);

    int result = (string_cmp(block_hash, block->hash) == 0);

    string_free(block_hash);
    string_free(hash);
    return result;
}

int block_check_transaction(
        const struct transaction_st *tran, const struct wallet_st *wallet_from, const struct integer_st *time,
        const struct currency_st *currency, int send_out) {
    if (integer_cmp(tran->balance_from, wallet_from->data->pre_balance) != 0 ||
        string_cmp(tran->hash_from, wallet_from->data->pre_hash) != 0)
        return 0;
    if (integer_is_neg(tran->balance) || integer_is_neg(tran->fee)) return 0;
    if (!integer_is_null(currency->our) && send_out) return 0;

    struct string_st *hash = string_new();
    struct integer_st *balance = integer_new();

    int result = 1;

    transaction_get_hash(tran, hash, time);
    if (!get_crypto_base(integer_get_ui(currency->crypto_base))._check_sign(tran->signature, tran->address_from, hash)) result = 0;


    if (integer_is_null(currency->our)) balance_outside(currency, wallet_from->data->address_outside, balance);
    if (send_out && integer_cmp(balance, tran->balance) < 0) result = 0;
    integer_add(balance, balance, tran->balance_from);
    integer_sub(balance, balance, tran->fee);
    integer_sub(balance, balance, wallet_from->smart->freeze);
    if (integer_cmp(balance, tran->balance) < 0) result = 0;

    string_free(hash);
    integer_free(balance);

    return result;
}

void block_send_benefit(const struct block_st *block, struct list_st *all_wallets) {
    if (integer_is_null(block->benefit)) return;

    struct integer_st *temp = integer_new();
    struct integer_st *freeze_sum = integer_new();
    struct activated_accounts *activated = activated_accounts_new();
    struct account_connections *connections = account_connections_new();
    activated_accounts_get(activated);
    activated_accounts_freeze(activated, freeze_sum);

    struct transaction_st *tran = NULL;
    struct wallet_st *wallet = NULL;
    struct wallet_st *wallet1 = NULL;
    struct wallet_st *wallet2 = NULL;
    struct list_st *sub_list = NULL;

    for (size_t i = 0; i < block->transactions->size; i++) {
        tran = block->transactions->data[i]->data;
        wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
        wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
        if (wallet1 == NULL || wallet2 == NULL) continue;

        if (string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 &&
            wallet1->smart->owner->size != 0)
            continue;

        for (size_t j = 0; j < activated->addresses->size; j++) {
            sub_list = activated->addresses->data[j]->data;
            account_connections_get(connections, tran->currency, sub_list->data[0]->data);
            if (list_is_null(connections->addresses)) continue;

            wallet = get_from_all_wallets(all_wallets, tran->currency, connections->addresses->data[0]->data);
            if (wallet == NULL) continue;

            integer_mul(temp, tran->fee, freeze_sum);
            integer_div(temp, temp, sub_list->data[1]->data);
            integer_add(wallet->data->pre_balance, wallet->data->pre_balance, temp);
        }
    }

    integer_free(temp);
    integer_free(freeze_sum);
    account_connections_free(connections);
    activated_accounts_free(activated);
}
void block_history_send_benefit(const struct block_history *block, struct list_st *all_wallets) {
    if (integer_is_null(block->benefit)) return;

    struct integer_st *temp = integer_new();
    struct integer_st *freeze_sum = integer_new();
    struct activated_accounts *activated = activated_accounts_new();
    struct account_connections *connections = account_connections_new();
    activated_accounts_get(activated);
    activated_accounts_freeze(activated, freeze_sum);

    struct transaction_st *tran = NULL;
    struct wallet_st *wallet = NULL;
    struct wallet_st *wallet1 = NULL;
    struct wallet_st *wallet2 = NULL;
    struct list_st *sub_list = NULL;

    for (size_t i = 0; i < block->transactions->size; i++) {
        tran = block->transactions->data[i]->data;
        wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
        wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
        if (wallet1 == NULL || wallet2 == NULL) continue;

        if (string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 &&
            wallet1->smart->owner->size != 0)
            continue;

        for (size_t j = 0; j < activated->addresses->size; j++) {
            sub_list = activated->addresses->data[j]->data;
            account_connections_get(connections, tran->currency, sub_list->data[0]->data);
            if (list_is_null(connections->addresses)) continue;

            wallet = get_from_all_wallets(all_wallets, tran->currency, connections->addresses->data[0]->data);
            if (wallet == NULL) continue;

            integer_mul(temp, tran->fee, freeze_sum);
            integer_div(temp, temp, sub_list->data[1]->data);
            integer_add(wallet->data->balance, wallet->data->balance, temp);
        }
    }

    integer_free(temp);
    integer_free(freeze_sum);
    account_connections_free(connections);
    activated_accounts_free(activated);
}

int block_check(const struct block_st *block) {
    if (block == NULL) return 0;
    if (!block_check_hash(block)) return 0;
    struct transaction_st *tran = NULL;
    struct currency_st *currency = NULL;
    struct wallet_st *wallet1 = NULL;
    struct wallet_st *wallet2 = NULL;

    int send_out = 0;
    int result = 1;

    if (!string_is_null(block->address_outside)) {
        if (list_is_null(block->transactions)) return 0;

        struct string_st *main_currency = string_new();
        string_set(main_currency, ((struct transaction_st *) block->transactions->data[0]->data)->currency);
        for (size_t i = 0; i < block->transactions->size && result; i++) {
            tran = block->transactions->data[0]->data;
            if (string_cmp(main_currency, tran->currency) != 0) result = 0;
        }

        if (currency_exist(main_currency)) {
            currency = currency_new();
            currency_get(currency, main_currency);
            if (!integer_is_null(currency->our)) result = 0;
            currency_free(currency);
        } else result = 0;

        string_free(main_currency);

        if (!result) return result;
        send_out = 1;
    }

    struct string_st *hash = string_new();
    struct integer_st *benefit = integer_new();
    struct list_st *all_currencies = list_new();
    struct list_st *all_wallets = list_new();


    for (size_t i = 0; i < block->transactions->size; i++) {
        tran = block->transactions->data[0]->data;
        currency = get_from_all_currency(all_currencies, tran->currency);
        wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
        wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
        if (currency == NULL || wallet1 == NULL || wallet2 == NULL) {
            result = 0;
            break;
        }
        if (!block_check_transaction(tran, wallet1, block->time, currency, send_out)) {
            result = 0;
            break;
        }
        {
            transaction_get_hash(tran, hash, block->time);
            integer_sub(wallet1->data->pre_balance, tran->balance_from, tran->balance);
            integer_add(wallet2->data->pre_balance, wallet2->data->pre_balance, tran->balance);
            if (!(string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 && wallet1->smart->owner->size != 0)) {
                integer_sub(wallet1->data->pre_balance, wallet1->data->pre_balance, tran->fee);
                integer_add(benefit, benefit, tran->fee);
            }
            string_set(wallet1->data->pre_hash, tran->hash_from);
            string_concat(wallet1->data->pre_hash, hash);
            get_hash_code(integer_get_si(currency->hash_type))._code(wallet1->data->pre_hash, wallet1->data->pre_hash);
        }
    }

    if (integer_cmp(benefit, block->benefit) != 0) result = 0;
    if (!string_is_null(block->smart_contract) && !block_smart_check(block->smart_contract)) result = 0;

    if (result) {
        block_send_benefit(block, all_wallets);
        for (size_t i = 0; i < all_wallets->size; i++) {
            wallet1 = all_wallets->data[i]->data;
            wallet_data_save_local(wallet1->data);
        }
    }

    string_free(hash);
    integer_free(benefit);
    list_free(all_wallets);
    list_free(all_currencies);

    return result;
}


#ifdef SUPER_NODE
void block_history_send_outside(const struct block_history *block, struct list_st *all_wallets, struct list_st *all_currencies){
    if(string_is_null(block->address_outside)) return;

    struct transaction_st *tran = NULL;
    struct currency_st *currency = NULL;
    struct wallet_st *wallet = NULL;

    {
        for (size_t i = 0; i < block->transactions->size; i++) {
            tran = block->transactions->data[0]->data;
            currency = get_from_all_currency(all_currencies, tran->currency);
            wallet = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
            if (currency == NULL || wallet == NULL) continue;
            send_outside(currency, wallet->data->address_outside, block->address_outside, tran->balance);
        }
    }

}
#endif



int block_history_save(const struct block_history *block) {
    if (block == NULL) return 0;
    if (!block_history_check_hash(block)) return 0;
    struct transaction_st *tran = NULL;
    struct currency_st *currency = NULL;
    struct wallet_st *wallet1 = NULL;
    struct wallet_st *wallet2 = NULL;

    int result = 1;
    int send_out = 0;

    if (!string_is_null(block->address_outside)) {
        if (list_is_null(block->transactions)) return 0;

        struct string_st *main_currency = string_new();
        string_set(main_currency, ((struct transaction_st *) block->transactions->data[0]->data)->currency);
        for (size_t i = 0; i < block->transactions->size && result; i++) {
            tran = block->transactions->data[0]->data;
            if (string_cmp(main_currency, tran->currency) != 0) result = 0;
        }

        if (currency_exist(main_currency)) {
            currency = currency_new();
            currency_get(currency, main_currency);
            if (!integer_is_null(currency->our)) result = 0;
            currency_free(currency);
        } else result = 0;

        string_free(main_currency);

        if (!result) return result;
        send_out = 1;
    }

    struct string_st *hash = string_new();
    struct integer_st *benefit = integer_new();
    struct list_st *all_currencies = list_new();
    struct list_st *all_wallets = list_new();


    for (size_t i = 0; i < block->transactions->size; i++) {
        tran = block->transactions->data[0]->data;
        currency = get_from_all_currency(all_currencies, tran->currency);
        wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
        wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
        if (currency == NULL || wallet1 == NULL || wallet2 == NULL) {
            result = 0;
            break;
        }
        if (!block_check_transaction(tran, wallet1, block->time, currency, send_out)) {
            result = 0;
            break;
        }
        {
            transaction_get_hash(tran, hash, block->time);
            integer_sub(wallet1->data->balance, tran->balance_from, tran->balance);
            integer_add(wallet2->data->balance, wallet2->data->balance, tran->balance);
            if (!(string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 && wallet1->smart->owner->size != 0)) {
                integer_sub(wallet1->data->balance, wallet1->data->balance, tran->fee);
                integer_add(benefit, benefit, tran->fee);
            }
            string_set(wallet1->data->hash, tran->hash_from);
            string_concat(wallet1->data->hash, hash);
            get_hash_code(integer_get_si(currency->hash_type))._code(wallet1->data->hash, wallet1->data->hash);
        }
    }

    if (integer_cmp(benefit, block->benefit) != 0) result = 0;
    if (!string_is_null(block->smart_contract) && !block_smart_check(block->smart_contract)) result = 0;


    if (result) {
        block_history_send_benefit(block, all_wallets);
        if (!string_is_null(block->smart_contract)) block_smart_make(block->smart_contract);
#ifdef SUPER_NODE
        block_history_send_outside(block, all_wallets, all_currencies);
#endif
        for (size_t i = 0; i < all_wallets->size; i++) {
            wallet1 = all_wallets->data[i]->data;
            wallet_data_save_local(wallet1->data);
        }
    }

    string_free(hash);
    integer_free(benefit);
    list_free(all_wallets);
    list_free(all_currencies);

    return result;
}

void block_history_restore(const struct block_history *block) {
    if (block == NULL) return;
    struct transaction_st *tran = NULL;
    struct currency_st *currency = NULL;
    struct wallet_st *wallet1 = NULL;
    struct wallet_st *wallet2 = NULL;

    struct string_st *hash = string_new();
    struct integer_st *benefit = integer_new();
    struct list_st *all_currencies = list_new();
    struct list_st *all_wallets = list_new();


    for (size_t i = 0; i < block->transactions->size; i++) {
        tran = block->transactions->data[0]->data;
        currency = get_from_all_currency(all_currencies, tran->currency);
        wallet1 = get_from_all_wallets(all_wallets, tran->currency, tran->address_from);
        wallet2 = get_from_all_wallets(all_wallets, tran->currency, tran->address_to);
        if (currency == NULL || wallet1 == NULL || wallet2 == NULL) continue;
        {
            transaction_get_hash(tran, hash, block->time);
            integer_sub(wallet1->data->balance, tran->balance_from, tran->balance);
            integer_add(wallet2->data->balance, wallet2->data->balance, tran->balance);
            if (!(string_cmp(wallet1->smart->owner, wallet2->smart->owner) == 0 && wallet1->smart->owner->size != 0)) {
                integer_sub(wallet1->data->balance, wallet1->data->balance, tran->fee);
                integer_add(benefit, benefit, tran->fee);
            }
            string_set(wallet1->data->hash, tran->hash_from);
            string_concat(wallet1->data->hash, hash);
            get_hash_code(integer_get_si(currency->hash_type))._code(wallet1->data->hash, wallet1->data->hash);
        }
    }

    block_history_send_benefit(block, all_wallets);
    if (!string_is_null(block->smart_contract)) block_smart_make(block->smart_contract);
    for (size_t i = 0; i < all_wallets->size; i++) {
        wallet1 = all_wallets->data[i]->data;
        string_set(wallet1->data->pre_hash, wallet1->data->hash);
        integer_set(wallet1->data->pre_balance, wallet1->data->balance);
        wallet_data_save_local(wallet1->data);
    }

    string_free(hash);
    integer_free(benefit);
    list_free(all_wallets);
    list_free(all_currencies);
}

void block_save(const struct block_st *block, int result) {
    if (block == NULL) return;

    int exist = block_remove(block->hash, block->time);
    exist |= !block_history_exist(block->hash, block->time);
    if (!exist) return;

    struct block_history *history = block_history_new();
    block_get_history(block, result, history);
    block_history_save_local(history);

    if (!result) return;
    result = block_history_save(history);
    if (!result) block_history_restore(history);

    // TODO run smart contract for all nodes

    block_history_free(history);
}


void blocks_restore() {
    struct list_st *history = list_new();
    struct generation *gen = generation_new();

    history_generation_request(history, NULL, NULL);
    list_sort(history);

    // TODO remove all existing data

    struct list_st *sub_list = NULL;
    for (size_t i = 0; i < history->size; i++) {
        sub_list = history->data[i]->data;
        generation_request(gen, sub_list->data[1]->data, sub_list->data[0]->data);
        generation_work(gen);
    }

    list_free(history);
    generation_free(gen);
    // TODo check this
}

