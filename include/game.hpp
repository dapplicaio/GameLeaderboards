#ifndef GAME_H
#define GAME_H

#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>
#include "atomicassets.hpp"

using namespace eosio;

class [[eosio::contract]] game : public contract
{
public:
  using contract::contract;

  // listening atomicassets transfer
  [[eosio::on_notify("atomicassets::transfer")]] void receive_asset_transfer(
      const name &from,
      const name &to,
      std::vector<uint64_t> &asset_ids,
      const std::string &memo);

  [[eosio::on_notify("tokencont::transfer")]] // tokencont change for your token contract
  void
  receive_token_transfer(
      const name &from,
      const name &to,
      const asset &quantity,
      const std::string &memo);

  [[eosio::action]] void claim(const name &owner, const uint64_t &farmingitem);

  [[eosio::action]] void upgradeitem(
      const name &owner,
      const uint64_t &item_to_upgrade,
      const uint8_t &next_level,
      const uint64_t &staked_at_farmingitem);

  [[eosio::action]] void upgfarmitem(const name &owner, const uint64_t &farmingitem_to_upgrade, const bool &staked);

  [[eosio::action]] void addblend(
      const std::vector<int32_t> blend_components,
      const int32_t resulting_item);

  [[eosio::action]] void setratio(const std::string &resource, const float &ratio);

  [[eosio::action]] void swap(const name &owner, const std::string &resource, const float &amount2swap);

  [[eosio::action]] void createvoting(
      const name &player,
      const std::string &resource_name,
      const float &new_ratio);

  [[eosio::action]] void vote(
      const name &player,
      const uint64_t &voting_id);

private:
  // scope: owner

  struct [[eosio::table]] lboard_j
  {
    name account;
    uint64_t points;

    uint64_t primary_key() const { return account.value; };
  };
  typedef multi_index<"lboards"_n, lboard_j> lboards_t;

  struct [[eosio::table]] staked_j
  {
    uint64_t asset_id;                  // item
    std::vector<uint64_t> staked_items; // farming items

    uint64_t primary_key() const { return asset_id; }
  };
  typedef multi_index<"staked"_n, staked_j> staked_t;

  // scope: owner
  struct [[eosio::table]] resources_j
  {
    uint64_t key_id;
    float amount;
    std::string resource_name;

    uint64_t primary_key() const { return key_id; }
  };
  typedef multi_index<"resources"_n, resources_j> resources_t;

  // scope:contract
  struct [[eosio::table]] blends_j
  {
    uint64_t blend_id;
    std::vector<int32_t> blend_components;
    int32_t resulting_item;

    uint64_t primary_key() const { return blend_id; }
  };
  typedef multi_index<"blends"_n, blends_j> blends_t;

  struct [[eosio::table]] balance_j
  {
    name owner;
    asset quantity;

    uint64_t primary_key() const { return owner.value; }
  };
  typedef multi_index<"balance"_n, balance_j> balance_t;

  struct [[eosio::table]] resourcecost_j
  {
    uint64_t key_id;
    std::string resource_name;
    float ratio; // if user swap 100 wood and ration is 25 it means that user will receive 4 tokens

    uint64_t primary_key() const { return key_id; }
  };
  typedef multi_index<"resourcecost"_n, resourcecost_j> resourcecost_t;

  struct [[eosio::table]] changeration_j
  {
    uint64_t voting_id;
    std::string resource_name;
    float new_ratio;
    std::map<name, asset> voted; // first is player name, second is voting power (in tokens)

    uint64_t primary_key() const { return voting_id; }
  };
  typedef multi_index<"changeration"_n, changeration_j> changeration_t;

  struct [[eosio::table]] avatars_j
  {
    name owner;
    std::vector<uint64_t> equipment;

    uint64_t primary_key() const { return owner.value; }
  };
  typedef multi_index<"avatarsc"_n, avatars_j> avatars_t;

  struct [[eosio::table]] stats_j
  {
    name owner;
    std::map<std::string, uint32_t> stats;

    uint64_t primary_key() const { return owner.value; }
  };
  typedef multi_index<"stats"_n, stats_j> stats_t;

  const uint64_t stringToUint64(const std::string &str);

  void stake_farmingitem(const name &owner, const uint64_t &asset_id);
  void stake_items(const name &owner, const uint64_t &farmingitem, const std::vector<uint64_t> &items_to_stake);

  void increase_owner_resources_balance(const name &owner, const std::map<std::string, float> &resources);
  void reduce_owner_resources_balance(const name &owner, const std::map<std::string, float> &resources);

  void increase_tokens_balance(const name &owner, const asset &quantity);

  const std::pair<std::string, float> claim_item(atomicassets::assets_t::const_iterator &assets_itr, const uint8_t &upgrade_percentage, const uint32_t &time_now, const std::map<std::string, uint32_t> &stats);

  void upgrade_item(
      atomicassets::assets_t::const_iterator &assets_itr,
      const uint8_t &upgrade_percentage,
      const name &owner,
      const uint8_t &new_level,
      const uint32_t &time_now,
      const std::map<std::string, uint32_t> &stats);

  void upgrade_farmingitem(atomicassets::assets_t::const_iterator &assets_itr, const name &owner);

  void blend(const name &owner, const std::vector<uint64_t> asset_ids, const uint64_t &blend_id);

  void set_avatar(const name &owner, const uint64_t &asset_id);
  void set_equipment_list(const name &owner, const std::vector<uint64_t> &asset_ids);
  void set_equipment_item(const name &owner, const uint64_t asset_id, std::vector<uint64_t> &assets_to_return, std::map<std::string, uint32_t> &equiped_types);
  std::map<std::string, uint32_t> get_stats(const name &owner);
  void recalculate_stats(const name &owner);

  void incr_lb_points(const name &lbname, const name &account, uint64_t points);
  void decr_lb_points(const name &lbname, const name &account, uint64_t points);
  void set_lb_points(const name &lbname, const name &account, uint64_t points);

  float get_mining_power(const uint64_t asset_id, const std::map<std::string, uint32_t> &stats);
  void update_mining_power_lb(const name &account);

  void tokens_transfer(const name &to, const asset &quantity);

  const int32_t get_upgrading_time(const uint8_t &end_level);

  // get mutable data from NFT
  atomicassets::ATTRIBUTE_MAP get_mdata(atomicassets::assets_t::const_iterator &assets_itr);
  // get immutable data from template of NFT
  atomicassets::ATTRIBUTE_MAP get_template_idata(const int32_t &template_id, const name &collection_name);
  // update mutable data of NFT
  void update_mdata(atomicassets::assets_t::const_iterator &assets_itr, const atomicassets::ATTRIBUTE_MAP &new_mdata, const name &owner);
};

#endif