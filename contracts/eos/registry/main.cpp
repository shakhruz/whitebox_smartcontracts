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

TABLE item
{
    uint64_t prim_key;
    name vaccount;
    std::vector<char> content;
    checksum256 file_hash;
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

ACTION regitem(item new_item)
{
    // require_auth(owner);
    require_vaccount(new_item.vaccount);

    cold_items_t items(_self, _self.value);
    items.emplace(_self, [&](auto &a) {
        a.prim_key = items.available_primary_key();
        a.vaccount = new_item.vaccount;
        a.content = new_item.content;
        a.file_hash = new_item.file_hash;
    });
};

ACTION warmupitem(name owner, uint64_t prim_key)
{
    cold_items_t items(_self, prim_key);
    auto existing = items.find(prim_key);
}

// ACTION clear() {
//     require_auth(_self);

//     cold_items_t items(_self, _self);
// };

VACCOUNTS_APPLY(((item)(regitem)))

CONTRACT_END((regitem)(warmupitem)(xdcommit)(regaccount)(xvinit));
// CONTRACT_END((regitem)(warmupitem))