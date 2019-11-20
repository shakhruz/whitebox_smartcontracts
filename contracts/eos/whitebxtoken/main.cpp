/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "whitebxtoken.hpp"

using namespace eosio;

[[eosio::action]] void whitebxtoken::create(name issuer, asset maximum_supply) {
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    eosio::check(sym.is_valid(), "invalid symbol name");
    eosio::check(maximum_supply.is_valid(), "invalid supply");
    eosio::check(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.code().raw());
    auto existing = statstable.find(sym.code().raw());
    eosio::check(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
};

[[eosio::action]] void whitebxtoken::issue(issue_args payload) // name to, asset quantity, string memo
{
    auto sym = payload.quantity.symbol;
    eosio::check(sym.is_valid(), "invalid symbol name");
    eosio::check(payload.memo.size() <= 256, "memo has more than 256 bytes");

    auto sym_name = sym.code().raw();
    stats statstable(_self, sym_name);
    auto existing = statstable.find(sym_name);
    eosio::check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio::check(payload.quantity.is_valid(), "invalid quantity");
    eosio::check(payload.quantity.amount > 0, "must issue positive quantity");

    eosio::check(payload.quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio::check(payload.quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, eosio::same_payer, [&](auto &s) {
        s.supply += payload.quantity;
    });

    add_cold_balance(payload.to, payload.quantity, st.issuer);
};

[[eosio::action]] void whitebxtoken::vtransfer(vtransfer_args payload) // name from, name to, asset quantity, string memo
{
    eosio::check(payload.from != payload.to, "cannot transfer to self");

    require_vaccount(payload.from);
    // eosio::check(is_account(payload.to), "to account does not exist");
    auto sym = payload.quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    eosio::check(payload.quantity.is_valid(), "invalid quantity");
    eosio::check(payload.quantity.amount > 0, "must transfer positive quantity");
    eosio::check(payload.quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio::check(payload.memo.size() <= 256, "memo has more than 256 bytes");

    sub_cold_balance(payload.from, payload.quantity);
    add_cold_balance(payload.to, payload.quantity, payload.from);
};

[[eosio::action]] void whitebxtoken::transfer(name from, name to, asset quantity, string memo) {
    require_auth(from);
    if (to != _self || from == _self || from == "eosio"_n || from == "eosio.stake"_n || from == to)
        return;
    if (memo == "seed transfer")
        return;
    if (memo.size() > 0)
    {
        name to_act = name(memo.c_str());
        // eosio::check(is_account(to_act), "The account name supplied is not valid");

        auto sym = quantity.symbol;
        eosio::symbol sym_name("MEOS", 4);
        eosio::symbol eos_symbol("EOS", 4);
        eosio::check(quantity.symbol == eos_symbol, "only EOS deposit is supported");

        stats statstable(_self, sym_name.code().raw());
        auto existing = statstable.find(sym_name.code().raw());
        eosio::check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto &st = *existing;

        eosio::check(quantity.is_valid(), "invalid quantity");
        eosio::check(quantity.amount > 0, "must issue positive quantity");

        asset meos_qty(quantity.amount * 1000, sym_name);

        add_balance(to_act, meos_qty, _self);

        // schedule moving these tokens to RAM and changing supply
        std::vector<char> payload(memo.begin(), memo.end());
        schedule_timer(_self, payload, 1);
    }
};

[[eosio::action]] void whitebxtoken::withdraw(withdraw_args payload) // name to, asset quantity
{
    require_vaccount(payload.from);
    // require_auth(to);

    auto sym = payload.quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    eosio::check(is_account(payload.to), "The account name supplied is not valid");

    eosio::check(payload.quantity.is_valid(), "invalid quantity");
    eosio::check(payload.quantity.amount > 0, "must withdraw positive quantity");
    eosio::check(payload.quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    require_recipient(payload.to);

    sub_cold_balance(payload.from, payload.quantity);
    statstable.modify(st, eosio::same_payer, [&](auto &s) {
        s.supply -= payload.quantity;
    });

    eosio::symbol eos_sym("EOS", 4);
    asset eos_qty(payload.quantity.amount / 1000, eos_sym);

    action(permission_level{_self, "active"_n}, EOS_TOKEN_CONTRACT, "transfer"_n,
           std::make_tuple(_self, payload.to, eos_qty, std::string("withdraw")))
        .send();
};

void whitebxtoken::add_balance(name owner, asset value, name ram_payer)
{
    accounts_t to_acnts(_self, owner.value);
    auto to = to_acnts.find(value.symbol.code().raw());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_acnts.modify(*to, ram_payer, [&](auto &a) {
            a.balance += value;
        });
    }
};

void whitebxtoken::sub_balance(name owner, asset value)
{
    accounts_t from_acnts(_self, owner.value);
    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    eosio::check(from.balance.amount >= value.amount, "overdrawn balance");
    if (from.balance.amount == value.amount)
    {
        from_acnts.erase(from);
    }
    else
    {
        from_acnts.modify(from, eosio::same_payer, [&](auto &a) {
            a.balance -= value;
        });
    }
};

void whitebxtoken::add_cold_balance(name owner, asset value, name ram_payer)
{
    cold_accounts_t to_acnts(_self, owner.value);
    auto to = to_acnts.find(value.symbol.code().raw());
    if (to == to_acnts.end())
    {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    }
    else
    {
        to_acnts.modify(*to, ram_payer, [&](auto &a) {
            a.balance += value;
        });
    }
};

void whitebxtoken::sub_cold_balance(name owner, asset value)
{
    cold_accounts_t from_acnts(_self, owner.value);
    const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
    eosio::check(from.balance.amount >= value.amount, "overdrawn balance");
    if (from.balance.amount == value.amount)
    {
        from_acnts.erase(from);
    }
    else
    {
        from_acnts.modify(from, eosio::same_payer, [&](auto &a) {
            a.balance -= value;
        });
    }
};

bool whitebxtoken::timer_callback(name timer, std::vector<char> payload, uint32_t seconds)
{
    string account_name(payload.begin(), payload.end());
    name to_act = name(account_name.c_str());

    accounts_t to_acnts(_self, to_act.value);
    eosio::symbol sym_name("MEOS", 4);
    const auto &to = to_acnts.get(sym_name.code().raw(), "no balance object found");

    stats statstable(_self, sym_name.code().raw());
    auto existing = statstable.find(sym_name.code().raw());
    eosio::check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto &st = *existing;

    statstable.modify(st, eosio::same_payer, [&](auto &s) {
        s.supply += to.balance;
    });

    add_cold_balance(to_act, to.balance, to_act);
    sub_balance(to_act, to.balance);

    return false;
};
