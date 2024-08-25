#pragma once
#include <queue>
#include "validator/manager-disk.h"
#include "validator/db/rootdb.hpp"

#include "IndexData.h"
#include "DataParser.h"
#include "EventProcessor.h"

enum ScannerMode { dbs_readonly, dbs_secondary };

class DbScanner: public td::actor::Actor {
private:
  std::string db_root_;
  ScannerMode mode_;
  std::optional<std::string> secondary_working_dir_;
  bool is_ready_ = false;
  ton::BlockSeqno last_known_seqno_{0};

  td::actor::ActorOwn<ton::validator::ValidatorManagerInterface> validator_manager_;
  td::actor::ActorOwn<ton::validator::RootDb> db_;
public:
  DbScanner(std::string db_root, ScannerMode mode, std::optional<std::string> secondary_working_dir = std::nullopt)
    : db_root_(db_root), mode_(mode), secondary_working_dir_(secondary_working_dir) {}

  void start_up() override;
  void alarm() override;

  void fetch_seqno(std::uint32_t mc_seqno, td::Promise<MasterchainBlockDataState> promise);
  void get_last_mc_seqno(td::Promise<ton::BlockSeqno> promise);
  void get_oldest_mc_seqno(td::Promise<ton::BlockSeqno> promise);
  void get_mc_block_handle(ton::BlockSeqno seqno, td::Promise<ton::validator::ConstBlockHandle> promise);
private:
  void catch_up_with_primary();
};

struct BlockIdExtHasher {
    std::size_t operator()(const ton::BlockIdExt& k) const {
        return std::hash<std::string>()(k.to_str());
    }
};
