#include <stdio.h>
#include "sakaar.h"
#include "crypto_base.h"


void print_obj(const struct object_st *res, int tabs);
void print_int(const struct integer_st *res) {
    printf("integer : ");
#ifdef USE_GMP
    gmp_printf("%Zx", res->mpz_int);
#else
    struct string_st *str = string_new();
    integer_get_str(res, str);
    for(int i=0;i<str->size;i++)printf("%c", str->data[i]);
    string_free(str);
#endif
    printf("\n");
}
void print_subint(const struct sub_integer *res) {
    printf("sub_integer : ");
    for (int i = 0; i < res->size; i++)printf("%d ", res->data[i]);
    printf("\n");
}
void print_str(const struct string_st *res) {
    printf("string (%zu): ", res->size);
    for (int i = 0; i < res->size; i++) printf("%c", res->data[i]);
    printf("\n");
}
void print_tlv(const struct string_st *res) {
    printf("tlv : ");
    for (int i = 0; i < res->size; i++)printf("%.2x ", (unsigned char) res->data[i]);
    printf("\n");
}
void print_list(const struct list_st *res, int tabs) {
    printf("list (%zu):\n", res->size);
    for (int i = 0; i < res->size; i++) {
        for (int _i = 0; _i < tabs; _i++) printf("\t");
        print_obj(res->data[i], tabs + 1);
    }
}
void print_obj(const struct object_st *res, int tabs) {
    printf("object : ");
    if (res->type == NONE_TYPE) printf("None");
    else if (res->type == STRING_TYPE) return print_str(res->data);
    else if (res->type == TLV_TYPE) return print_tlv(res->data);
    else if (res->type == INTEGER_TYPE) return print_int(res->data);
    else if (res->type == SUB_INTEGER_TYPE) return print_subint(res->data);
    else if (res->type == OBJECT_TYPE) return print_obj(res->data, tabs);
    else if (res->type == LIST_TYPE) return print_list(res->data, tabs);
}



void my_cre_block_smart_account_create(const struct string_st *login, int hash_type, int crypto_type, int crypto_base, const struct string_st *key);
void my_account_register(const struct string_st *login, const struct string_st *password){
    if(main_config == NULL) main_config = config_new();
    struct string_st *key = string_new();
    struct string_st *hash = string_new();

    get_crypto_base(1)._from_string(key, password);
    my_cre_block_smart_account_create(login, 1, 1, 1, key);

    string_free(key);
    string_free(hash);
}

void my_cre_block_smart_currency_create(const struct string_st *name, const struct account_st *account, const struct string_st *key, const struct string_st *smc, const struct string_st *info, const struct integer_st *our, const struct integer_st *product, const struct integer_st *hash_type, const struct integer_st *crypto_type, const struct integer_st *crypto_base, const struct integer_st *balance);
void my_currency_create(const struct string_st *name, const struct string_st *smc, const struct integer_st *balance){
    if (main_config == NULL) main_config = config_new();

    struct integer_st *_our = integer_new();
    struct integer_st *_product = integer_new();
    struct integer_st *_hash_type = integer_new();
    struct integer_st *_crypto_type = integer_new();
    struct integer_st *_crypto_base = integer_new();

    integer_set_ui(_our, 1);
    integer_set_ui(_product, 0);
    integer_set_ui(_hash_type, 1);
    integer_set_ui(_crypto_type, 1);
    integer_set_ui(_crypto_base, 1);

    if (!integer_is_null(balance) && main_config->account->address->size != 0)
        my_cre_block_smart_currency_create(name, main_config->account, main_config->key, smc, NULL, _our, _product,
                                           _hash_type, _crypto_type, _crypto_base, balance);
    else
        my_cre_block_smart_currency_create(name, NULL, NULL, smc, NULL, _our, _product, _hash_type, _crypto_type,
                                           _crypto_base, NULL);

    integer_free(_our);
    integer_free(_product);
    integer_free(_hash_type);
    integer_free(_crypto_type);
    integer_free(_crypto_base);
}


void my_cre_block_smart_wallet_connect(const struct string_st *currency, const struct string_st *name, const struct string_st *w_key, const struct string_st *smc, const struct account_st *account, const struct string_st *key);
void my_wallet_create(const struct string_st *currency, const struct string_st* name, const struct string_st *smart_contract){
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;

    struct currency_st *Currency = currency_new();
    struct string_st *w_key = string_new();
    currency_get(Currency, currency);

    get_crypto_base(integer_get_ui(Currency->crypto_base))._generate(w_key);
    my_cre_block_smart_wallet_connect(currency, name, w_key, smart_contract, main_config->account, main_config->key);

    currency_free(Currency);
    string_free(w_key);
}

void my_cre_block_smart_account_freeze(const struct string_st *address, const struct integer_st *balance, const struct account_st *account, const struct string_st *key);
void my_account_freeze(const struct string_st *address, const struct integer_st *balance) {
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    my_cre_block_smart_account_freeze(address, balance, main_config->account, main_config->key);
}

void my_cre_block_smart_account_activate(const struct account_st *account, const struct string_st *key);
void my_account_activate(){
    if (main_config == NULL) main_config = config_new();
    if (account_is_null(main_config->account)) return;
    my_cre_block_smart_account_activate(main_config->account, main_config->key);
}

void make_first(){
    main_config->_is_server = 1;


    struct string_st *login = string_new();
    struct string_st *curr = string_new();
    struct integer_st *balance = integer_new();
    struct string_st *w_address1 = string_new();
    struct string_st *w_address2 = string_new();

    string_set_str(login, "JustSteve", 9);
    string_set_str(curr, "SKR", 3);
    _integer_set_str(balance, "D3C21BCECCEDA000000000", 22);

    my_account_register(login, NULL);
    config_account_log_in(login, NULL);
    my_currency_create(curr, NULL, balance);
    my_wallet_create(curr, NULL, curr);

    {
        struct account_connections *connections = account_connections_new();
        account_connections_get(connections, curr, main_config->account->address);

        string_set(w_address1, connections->addresses->data[0]->data);

        account_connections_free(connections);
    }
    _integer_set_str(balance, "100", 3);
    my_account_freeze(w_address1, balance);
    my_account_activate();

    {
        struct string_st *w_key = string_new();
        get_crypto_base(1)._generate(w_key);
        get_crypto_base(1)._get_public(w_key, w_address2);
        string_free(w_key);
    }
    //
    //    for(int i=0;i<100;i++)
    //    {
    //        struct list_st *trans = list_new();
    //        struct object_st *obj = object_new();
    //        struct block_st *block = block_new();
    //
    //        object_set_type(obj, PRE_TRANSACTION_TYPE);
    //        config_make_pre_transaction(obj->data, w_address1, w_address2, curr, balance, balance);
    //        list_append(trans, obj);
    //
    //        make_block(block, trans, NULL, NULL);
    //        config_block_create(block);
    //
    //
    //        object_free(obj);
    //        list_free(trans);
    //        block_free(block);
    //    }


    string_free(login);
    string_free(curr);
    integer_free(balance);
    string_free(w_address1);
    string_free(w_address2);
}






int block_check(const struct block_st *block);
void blocks_restore();

int main() {
    struct string_st *str = string_new();
    string_set_str(str, "123", 3);
    sha256_code._code(str, str);
    print_str(str);

    struct string_st *tlv = string_new();

    {
        struct integer_st *num = integer_new();
        integer_set_str(num, str);
        integer_get_tlv(num, tlv);
    }
    print_tlv(tlv);

    {
        struct integer_st *num = integer_new();
        integer_set_tlv(num, tlv);
        integer_get_str(num, str);
    }
    print_str(str);




//    main_config = config_new();
//    make_first();
//
//    config_server_start();
//    sleep(10000);
//
//    config_server_close();
//    sleep(1);
//
//    config_free(main_config);
    printf("LEFT : %zu\n", mem_ctx.filled);
}
//    struct skr_object *expr_obj = object_new();
//    object_set_type(expr_obj, EXPR_SELF);
//    {
//        struct file_parser *F_parser = file_parser_new();
//        struct token_parser *T_parser = token_parser_new();
//        file_parser_from_file(F_parser, "text.txt");
//        tokenize(F_parser);
//
//        token_parser_from_file_parser(T_parser, F_parser);
//        file_parser_free(F_parser);
//
//        token_analyzer(T_parser, expr_obj->data);
//        token_parser_free(T_parser);
//    }
//    obj_print(expr_obj, 0);
//    struct state_st *state = state_new();
//    clock_t begin = clock();
//    stmt_run(expr_obj, state);
//    clock_t end = clock();
//    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//    list_print(state->memory_names,0);
//    list_print(state->memory,0);
//    printf("TIME : %f\n", time_spent);
//    object_free(expr_obj);
//    state_free(state);

// Сделать сериализацию обектов
// У обектов будет две переменые _encode _decode
// работать так же как с обычными обектами

