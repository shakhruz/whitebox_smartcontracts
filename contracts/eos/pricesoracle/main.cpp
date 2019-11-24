#define VACCOUNTS_DELAYED_CLEANUP 120

#include "../dappservices/oracle.hpp"
#include "../dappservices/multi_index.hpp"
#include "../dappservices/ipfs.hpp"
#include "../dappservices/vaccounts.hpp"
#include "../dappservices/log.hpp"
#include "../dappservices/plist.hpp"
#include "../dappservices/plisttree.hpp"
#include "../dappservices/cron.hpp"

#define DAPPSERVICES_ACTIONS()  \
  XSIGNAL_DAPPSERVICE_ACTION    \
  LOG_DAPPSERVICE_ACTIONS       \
  IPFS_DAPPSERVICE_ACTIONS      \
  VACCOUNTS_DAPPSERVICE_ACTIONS \
  ORACLE_DAPPSERVICE_ACTIONS    \
  CRON_DAPPSERVICE_ACTIONS

#define DAPPSERVICE_ACTIONS_COMMANDS() \
  IPFS_SVC_COMMANDS()                  \
  LOG_SVC_COMMANDS()                   \
  VACCOUNTS_SVC_COMMANDS()             \
  ORACLE_SVC_COMMANDS()                \
  CRON_SVC_COMMANDS()

#define CONTRACT_NAME() pricesoracle

namespace eosiosystem
{
class system_contract;
}

using std::string;

CONTRACT_START()

bool timer_callback(name timer, std::vector<char> payload, uint32_t seconds);

public:
struct getrate_struct
{
  name vaccount;
  symbol_code sym;
  std::vector<char> uri;
  EOSLIB_SERIALIZE(getrate_struct, (vaccount)(sym)(uri))
};

[[eosio::action]] void getrate(getrate_struct payload);

// private:
TABLE price
{
  uint64_t prim_key;
  symbol_code sym;
  name vaccount;
  string uri;
  string price;
  block_timestamp timestamp;

  auto primary_key() const { return prim_key; }
  // auto primary_key() const { return supply.symbol.code().raw(); }
};

TABLE shardbucket
{
  std::vector<char> shard_uri;
  uint64_t shard;
  uint64_t primary_key() const { return shard; }
};

typedef dapp::multi_index<"prices"_n, price> prices_table;
typedef eosio::multi_index<".prices"_n, price> prices_table_v_abi;
typedef eosio::multi_index<"prices"_n, shardbucket> prices_table_abi;

VACCOUNTS_APPLY(((getrate_struct)(getrate)))
CONTRACT_END((getrate)(xdcommit)(regaccount)(xvinit))

[[eosio::action]] void pricesoracle::getrate(getrate_struct payload)
{
  // require_auth(get_self());
  // require_vaccount(payload.vaccount);

  string res;
  // std::vector<char> url(payload.uri.begin(), payload.uri.end());
  getURI(payload.uri, [&](auto &results) {
    res = string(results[0].result.begin(), results[0].result.end());
    return results[0].result;
  });

  prices_table to_prices(_self, payload.vaccount.value);
  to_prices.emplace(payload.vaccount, [&](auto &p) {
    p.prim_key = to_prices.available_primary_key();
    p.timestamp = eosio::current_block_time();
    p.vaccount = payload.vaccount;
    p.sym = payload.sym;
    // p.uri = url;
    p.price = res;
  });
};

bool pricesoracle::timer_callback(name timer, std::vector<char> payload, uint32_t seconds)
{
  return false;
};