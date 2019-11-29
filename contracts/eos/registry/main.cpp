#define VACCOUNTS_DELAYED_CLEANUP 120

#include "../dappservices/multi_index.hpp"
#include "../dappservices/ipfs.hpp"
#include "../dappservices/vaccounts.hpp"

#define DAPPSERVICES_ACTIONS() \
    XSIGNAL_DAPPSERVICE_ACTION \
    IPFS_DAPPSERVICE_ACTIONS   \
    VACCOUNTS_DAPPSERVICE_ACTIONS

#define DAPPSERVICE_ACTIONS_COMMANDS() \
    IPFS_SVC_COMMANDS()                \
    VACCOUNTS_SVC_COMMANDS()

#define CONTRACT_NAME() registry
using std::string;

CONTRACT_START()

public:
struct regitem_args
{
    name vaccount;
    std::vector<char> content;
    string hash;

    EOSLIB_SERIALIZE(regitem_args, (vaccount)(content)(hash))
};

struct clear_args
{
    uint64_t prim_key;
    name vaccount;

    EOSLIB_SERIALIZE(clear_args, (prim_key)(vaccount))
};

// private:
TABLE item
{
    uint64_t prim_key;
    name vaccount;
    std::vector<char> content;
    string hash;
    uint64_t primary_key() const { return prim_key; }
};

typedef dapp::multi_index<"vitems"_n, item> cold_items_t;
typedef eosio::multi_index<".vitems"_n, item> cold_items_t_v_abi;
TABLE shardbucket
{
    std::vector<char> shard_uri;
    uint64_t shard;
    uint64_t primary_key() const { return shard; }
};
typedef eosio::multi_index<"vitems"_n, shardbucket> cold_items_t_abi;

public:
ACTION save(regitem_args payload)
{
    // require_auth(owner);
    require_vaccount(payload.vaccount);

    cold_items_t items(_self, _self.value);
    items.emplace(_self, [&](auto &a) {
        a.prim_key = items.available_primary_key();
        a.vaccount = payload.vaccount;
        a.content = payload.content;
        a.hash = payload.hash;
    });
};

ACTION clear(clear_args payload)
{
    require_vaccount(payload.vaccount);
    // require_auth(get_self());

    cold_items_t items(get_self(), get_self().value);
    const auto &item = items.get(payload.prim_key, "item with key not found");
    items.erase(item);
}

VACCOUNTS_APPLY(((regitem_args)(save))((clear_args)(clear)))

CONTRACT_END((save)(xdcommit)(regaccount)(xvinit)(clear));
// CONTRACT_END((regitem)(warmupitem))