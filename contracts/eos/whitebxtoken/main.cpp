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
    eosio::check(is_account(payload.to), "to account does not exist");
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
        auto sym_name = name("MEOS");
        eosio::check(sym.code().raw() == name("EOS").value, "only EOS deposit is supported");

        stats statstable(_self, sym_name.value);
        auto existing = statstable.find(sym_name.value);
        eosio::check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto &st = *existing;

        eosio::check(quantity.is_valid(), "invalid quantity");
        eosio::check(quantity.amount > 0, "must issue positive quantity");

        statstable.modify(st, eosio::same_payer, [&](auto &s) {
            s.supply += quantity * 1000;
        });

        add_balance(to_act, quantity * 1000, from);
    }
};

[[eosio::action]] void whitebxtoken::withdraw(withdraw_args payload) // name to, asset quantity
{
    require_vaccount(payload.from);
    // require_auth(to);

    auto sym = payload.quantity.symbol.code().raw();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    // require_recipient(payload.to);

    eosio::check(payload.quantity.is_valid(), "invalid quantity");
    eosio::check(payload.quantity.amount > 0, "must withdraw positive quantity");
    eosio::check(payload.quantity.symbol == st.supply.symbol, "symbol precision mismatch");

    sub_cold_balance(payload.from, payload.quantity);
    statstable.modify(st, eosio::same_payer, [&](auto &s) {
        s.supply -= payload.quantity;
    });
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
}

// extern "C"
// {
//     void apply(uint64_t receiver, uint64_t code, uint64_t action)
//     {
//         whitebxtoken _whitebxtoken(receiver);
//         if (code == name("eosio.token").value && action == name("transfer").value)
//         {
//             execute_action(name(receiver), name(code), &addressbook::transferext);
//         }
//     }
// };
