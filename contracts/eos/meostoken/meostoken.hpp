/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#define VACCOUNTS_DELAYED_CLEANUP 120

#include "../dappservices/vaccounts.hpp"
#include "../dappservices/ipfs.hpp"

#include "../dappservices/log.hpp"
#include "../dappservices/plist.hpp"
#include "../dappservices/plisttree.hpp"
#include "../dappservices/multi_index.hpp"
#include "../dappservices/cron.hpp"

#define DAPPSERVICES_ACTIONS()    \
    XSIGNAL_DAPPSERVICE_ACTION    \
    LOG_DAPPSERVICE_ACTIONS       \
    IPFS_DAPPSERVICE_ACTIONS      \
    VACCOUNTS_DAPPSERVICE_ACTIONS \
    CRON_DAPPSERVICE_ACTIONS

#define DAPPSERVICE_ACTIONS_COMMANDS() \
    IPFS_SVC_COMMANDS()                \
    LOG_SVC_COMMANDS()                 \
    VACCOUNTS_SVC_COMMANDS()           \
    CRON_SVC_COMMANDS()

#define CONTRACT_NAME() meostoken
#define MEOS_SYM symbol(name("MEOS"))
#define EOS_TOKEN_CONTRACT name("eosio.token")

namespace eosiosystem
{
class system_contract;
}

using std::string;

CONTRACT_START()

bool timer_callback(name timer, std::vector<char> payload, uint32_t seconds);

public:
struct vtransfer_args
{
    name from;
    name to;
    asset quantity;
    string memo;

    EOSLIB_SERIALIZE(vtransfer_args, (from)(to)(quantity)(memo))
};

struct issue_args
{
    name to;
    asset quantity;
    string memo;

    EOSLIB_SERIALIZE(issue_args, (to)(quantity)(memo))
};

struct withdraw_args
{
    name from;
    name to;
    asset quantity;

    EOSLIB_SERIALIZE(withdraw_args, (from)(to)(quantity))
};

[[eosio::action]] void create(name issuer, asset maximum_supply);
[[eosio::action]] void issue(issue_args payload);
[[eosio::action]] void withdraw(withdraw_args payload);
[[eosio::action]] void vtransfer(vtransfer_args payload);
[[eosio::action]] void transfer(name from, name to, asset quantity, string memo);

inline asset get_supply(symbol_code sym) const;
inline asset get_balance(name owner, symbol_code sym) const;

// private:
TABLE account
{
    asset balance;
    uint64_t primary_key() const { return balance.symbol.code().raw(); }
};

TABLE vramaccounts
{
    asset balance;
    uint64_t primary_key() const { return balance.symbol.code().raw(); }
};

TABLE currency_stats
{
    asset supply;
    asset max_supply;
    name issuer;

    uint64_t primary_key() const { return supply.symbol.code().raw(); }
};

TABLE shardbucket
{
    std::vector<char> shard_uri;
    uint64_t shard;
    uint64_t primary_key() const { return shard; }
};

typedef dapp::multi_index<"vaccounts"_n, vramaccounts> cold_accounts_t;
typedef eosio::multi_index<".vaccounts"_n, vramaccounts> cold_accounts_t_v_abi;
typedef eosio::multi_index<"vaccounts"_n, shardbucket> cold_accounts_t_abi;
typedef eosio::multi_index<"accounts"_n, account> accounts_t;
typedef eosio::multi_index<"stat"_n, currency_stats> stats;

void sub_balance(name owner, asset value);
void add_balance(name owner, asset value, name ram_payer);
void sub_cold_balance(name owner, asset value);
void add_cold_balance(name owner, asset value, name ram_payer);

VACCOUNTS_APPLY(((issue_args)(issue))((withdraw_args)(withdraw))((vtransfer_args)(vtransfer)))

// CONTRACT_END((create)(issue)(vtransfer)(withdraw)(regaccount)(xdcommit)(xvinit))
}
;
EOSIO_DISPATCH_SVC_TRX(CONTRACT_NAME(), (create)(issue)(vtransfer)(withdraw)(regaccount)(xdcommit)(xvinit))

asset meostoken::get_supply(symbol_code sym) const
{
    stats statstable(_self, sym.raw());
    const auto &st = statstable.get(sym.raw());
    return st.supply;
}

asset meostoken::get_balance(name owner, symbol_code sym) const
{
    accounts_t accountstable(_self, owner.value);
    const auto &ac = accountstable.get(sym.raw());
    return ac.balance;
}