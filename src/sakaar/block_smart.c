#include "sakaar.h"

int block_smart_check(const struct string_st *res){
    switch (tlv_get_tag(res->data)) {
        case TLV_SMRT_CURRENCY_CREATE:
            return che_block_smart_currency_create(res);
        case TLV_SMRT_WALLET_CONNECT:
            return che_block_smart_wallet_connect(res);
        case TLV_SMRT_WALLET_DISCONNECT:
            return che_block_smart_wallet_disconnect(res);
        case TLV_SMRT_ACCOUNT_CREATE:
            return che_block_smart_account_create(res);
        case TLV_SMRT_ACCOUNT_FREEZE:
            return che_block_smart_account_freeze(res);
        case TLV_SMRT_ACCOUNT_ACTIVATE:
            return che_block_smart_account_activate(res);
        case TLV_SMRT_ACCOUNT_DEACTIVATE:
            return che_block_smart_account_deactivate(res);
    }
    return 0;
}
void block_smart_make(const struct string_st *res){
    switch (tlv_get_tag(res->data)) {
        case TLV_SMRT_CURRENCY_CREATE:
            return mk_block_smart_currency_create(res);
        case TLV_SMRT_WALLET_CONNECT:
            return mk_block_smart_wallet_connect(res);
        case TLV_SMRT_WALLET_DISCONNECT:
            return mk_block_smart_wallet_disconnect(res);
        case TLV_SMRT_ACCOUNT_CREATE:
            return mk_block_smart_account_create(res);
        case TLV_SMRT_ACCOUNT_FREEZE:
            return mk_block_smart_account_freeze(res);
        case TLV_SMRT_ACCOUNT_ACTIVATE:
            return mk_block_smart_account_activate(res);
        case TLV_SMRT_ACCOUNT_DEACTIVATE:
            return mk_block_smart_account_deactivate(res);
    }
}
