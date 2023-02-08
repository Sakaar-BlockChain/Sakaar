#ifndef SMART_SAKAAR_H
#define SMART_SAKAAR_H

#include <sys/stat.h>
#include <dirent.h>

#include "sakaar/account.h"
#include "sakaar/account_connections.h"
#include "sakaar/activated_accounts.h"
#include "sakaar/block.h"
#include "sakaar/block_history.h"
#include "sakaar/block_smart.h"
#include "sakaar/config.h"
#include "sakaar/currency.h"
#include "sakaar/generation.h"
#include "sakaar/transaction.h"
#include "sakaar/wallet.h"
#include "sakaar/wallet_data.h"
#include "sakaar/wallet_smart.h"


#define TLV_SMRT_CURRENCY_CREATE     0x3F10
#define TLV_SMRT_WALLET_CREATE       0x3F20
#define TLV_SMRT_WALLET_CONNECT      0x3F21
#define TLV_SMRT_WALLET_DISCONNECT   0x3F22
#define TLV_SMRT_ACCOUNT_CREATE      0x3F30
#define TLV_SMRT_ACCOUNT_FREEZE      0x3F31
#define TLV_SMRT_ACCOUNT_ACTIVATE    0x3F33
#define TLV_SMRT_ACCOUNT_DEACTIVATE  0x3F34

#define TLV_GEN_ACCOUNT_USERNAME    0x3F32
#define TLV_GEN_BLOCK_CREATE        0x3F40
#define TLV_GEN_BLOCK_RESULT        0x3F41

#define TLV_REQ_ACCOUNT             0x5F30
#define TLV_REQ_ACCOUNT_CONN        0x5F31
#define TLV_REQ_ACTIVE_ACC          0x5F32
#define TLV_REQ_BLOCK               0x5F40
#define TLV_REQ_BLOCK_HISTORY       0x5F41
#define TLV_REQ_BLOCK_HISTORY_LIST  0x5F42
#define TLV_REQ_CURRENCY            0x5F10
#define TLV_REQ_GENERATION          0x5F50
#define TLV_REQ_GENERATION_LIST     0x5F51
#define TLV_REQ_WALLET              0x5F20
#define TLV_REQ_WALLET_DATA         0x5F21
#define TLV_REQ_WALLET_SMART        0x5F22

#define TLV_CONFIG          0x5F50

#endif //SMART_SAKAAR_H
