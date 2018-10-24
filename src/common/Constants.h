/*
 * Copyright (c) 2018 Zilliqa
 * This source code is being disclosed to you solely for the purpose of your
 * participation in testing Zilliqa. You may view, compile and run the code for
 * that purpose and pursuant to the protocols and algorithms that are programmed
 * into, and intended by, the code. You may not do anything else with the code
 * without express permission from Zilliqa Research Pte. Ltd., including
 * modifying or publishing the code (or any part of it), and developing or
 * forming another public or private blockchain network. This source code is
 * provided 'as is' and no warranties are given as to title or non-infringement,
 * merchantability or fitness for purpose and, to the extent permitted by law,
 * all liability for your use of the code is disclaimed. Some programs in this
 * code are governed by the GNU General Public License v3.0 (available at
 * https://www.gnu.org/licenses/gpl-3.0.en.html) ('GPLv3'). The programs that
 * are governed by GPLv3.0 are those programs that are located in the folders
 * src/depends and tests/depends and which include a reference to GPLv3 in their
 * program files.
 */

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "depends/common/FixedHash.h"

using BlockHash = dev::h256;

// Data sizes
const unsigned int COMMON_HASH_SIZE = 32;
const unsigned int ACC_ADDR_SIZE = 20;
const unsigned int TRAN_HASH_SIZE = 32;
const unsigned int TRAN_SIG_SIZE = 64;
const unsigned int BLOCK_HASH_SIZE = 32;
const unsigned int BLOCK_SIG_SIZE = 64;
const unsigned int STATE_HASH_SIZE = 32;
const unsigned int RESERVED_FIELD_SIZE = 128;

// Numeric types sizes
const unsigned int UINT256_SIZE = 32;
const unsigned int UINT128_SIZE = 16;
const unsigned int INT256_SIZE = 32;

// Cryptographic sizes
const unsigned int PRIV_KEY_SIZE = 32;
const unsigned int PUB_KEY_SIZE = 33;
const unsigned int SIGNATURE_CHALLENGE_SIZE = 32;
const unsigned int SIGNATURE_RESPONSE_SIZE = 32;
const unsigned int COMMIT_SECRET_SIZE = 32;
const unsigned int COMMIT_POINT_SIZE = 33;
const unsigned int CHALLENGE_SIZE = 32;
const unsigned int RESPONSE_SIZE = 32;

const unsigned int BLOCKCHAIN_SIZE = 50;

// Number of nodes sent from lookup node to newly joined node
const unsigned int SEED_PEER_LIST_SIZE = 20;

// Transaction body sharing

const unsigned int NUM_VACUOUS_EPOCHS = 1;

// Networking and mining
const unsigned int POW_SIZE = 32;
const unsigned int IP_SIZE = 16;
const unsigned int PORT_SIZE = 4;

const unsigned int NUM_PEERS_TO_SEND_IN_A_SHARD = 20;
const unsigned int SERVER_PORT = 4201;

// Number of initial ds epoch number, including genesis epoch
const unsigned int INIT_DS_EPOCH_NUM = 2;

// Testing parameters

// Metadata type
enum MetaType : unsigned char {
  STATEROOT = 0x00,
  DSINCOMPLETED,
  LATESTACTIVEDSBLOCKNUM,
};

// Sync Type
enum SyncType : unsigned int {
  NO_SYNC = 0,
  NEW_SYNC,
  NORMAL_SYNC,
  DS_SYNC,
  LOOKUP_SYNC,
};

const std::string RAND1_GENESIS =
    "2b740d75891749f94b6a8ec09f086889066608e4418eda656c93443e8310750a";
const std::string RAND2_GENESIS =
    "e8cc9106f8a28671d91e2de07b57b828934481fadf6956563b963bb8e5c266bf";

const std::string REMOTE_TEST_DIR = "zilliqa-test";
const std::string PERSISTENCE_PATH = "persistence";
const std::string TX_BODY_SUBDIR = "txBodies";

const std::string DS_KICKOUT_MSG = "KICKED OUT FROM DS";
const std::string DS_LEADER_MSG = "DS LEADER NOW";
const std::string DS_BACKUP_MSG = "DS BACKUP NOW";

extern const std::string SCILLA_ROOT;
extern const std::string SCILLA_BINARY;
extern const std::string SCILLA_FILES;
extern const std::string SCILLA_LOG;
extern const std::string SCILLA_LIB;
extern const std::string INIT_JSON;
extern const std::string INPUT_STATE_JSON;
extern const std::string INPUT_BLOCKCHAIN_JSON;
extern const std::string INPUT_MESSAGE_JSON;
extern const std::string OUTPUT_JSON;
extern const std::string INPUT_CODE;
extern const std::string TXN_PATH;
extern const std::string DB_HOST;

extern const unsigned int MSG_VERSION;
extern const unsigned int DS_MULTICAST_CLUSTER_SIZE;
extern const unsigned int COMM_SIZE;
extern const unsigned int NUM_DS_ELECTION;
extern const unsigned int POW_WINDOW_IN_SECONDS;
extern const unsigned int NEW_NODE_SYNC_INTERVAL;
extern const unsigned int POW_SUBMISSION_TIMEOUT;
extern const unsigned int DS_POW_DIFFICULTY;
extern const unsigned int POW_DIFFICULTY;
extern const unsigned int POW_SUBMISSION_LIMIT;
extern const unsigned int MICROBLOCK_TIMEOUT;
extern const unsigned int VIEWCHANGE_TIME;
extern const unsigned int VIEWCHANGE_EXTRA_TIME;
extern const unsigned int CONSENSUS_MSG_ORDER_BLOCK_WINDOW;
extern const unsigned int CONSENSUS_OBJECT_TIMEOUT;
extern const unsigned int FETCHING_MISSING_DATA_TIMEOUT;
extern const unsigned int DS_MICROBLOCK_CONSENSUS_OBJECT_TIMEOUT;
extern const unsigned int NUM_FINAL_BLOCK_PER_POW;
extern const unsigned int NUM_DS_KEEP_TX_BODY;
extern const uint32_t MAXMESSAGE;
extern const unsigned int MICROBLOCK_GAS_LIMIT;
extern const unsigned int TX_SHARING_CLUSTER_SIZE;
extern const unsigned int NEW_NODE_POW_DELAY;
extern const unsigned int POST_VIEWCHANGE_BUFFER;
extern const unsigned int CONTRACT_CREATE_GAS;
extern const unsigned int CONTRACT_INVOKE_GAS;
extern const unsigned int NORMAL_TRAN_GAS;
extern const unsigned int COINBASE_REWARD;
extern const unsigned int DEBUG_LEVEL;
extern const unsigned int BROADCAST_INTERVAL;
extern const unsigned int BROADCAST_EXPIRY;
extern const unsigned int TX_DISTRIBUTE_TIME_IN_MS;
extern const unsigned int FINALBLOCK_DELAY_IN_MS;
extern const unsigned int NUM_TXN_TO_SEND_PER_ACCOUNT;
extern const unsigned int NUM_NODES_TO_SEND_LOOKUP;
extern const unsigned int MAX_INDEXES_PER_TXN;
extern const unsigned int SENDQUEUE_SIZE;
extern const unsigned int MSGQUEUE_SIZE;
extern const unsigned int POW_CHANGE_PERCENT_TO_ADJ_DIFF;
extern const unsigned int FALLBACK_INTERVAL_STARTED;
extern const unsigned int FALLBACK_INTERVAL_WAITING;
extern const unsigned int FALLBACK_CHECK_INTERVAL;
extern const unsigned int FALLBACK_EXTRA_TIME;
extern const unsigned int FALLBACK_TEST_EPOCH;
extern const unsigned int NUM_NODE_INCR_DIFFICULTY;
extern const unsigned int MAX_SHARD_NODE_NUM;
extern const unsigned int MAX_ROUNDS_IN_BSTATE;
extern const unsigned int MAX_ROUNDS_IN_CSTATE;
extern const unsigned int MAX_TOTAL_ROUNDS;
extern const unsigned int MAX_NEIGHBORS_PER_ROUND;
extern const unsigned int ROUND_TIME_IN_MS;
extern const unsigned int NUM_MICROBLOCK_SENDERS;
extern const unsigned int NUM_MICROBLOCK_GOSSIP_RECEIVERS;
extern const unsigned int NUM_FINALBLOCK_GOSSIP_RECEIVERS_PER_SHARD;
extern const unsigned int HEARTBEAT_INTERVAL_IN_SECONDS;
extern const unsigned int ROUND_TIME_IN_MS;
extern const unsigned int TERMINATION_COUNTDOWN_IN_SECONDS;
extern const unsigned int DS_DELAY_WAKEUP_IN_SECONDS;
extern const unsigned int SHARD_DELAY_WAKEUP_IN_SECONDS;
extern const unsigned int NUM_FORWARDED_BLOCK_RECEIVERS_PER_SHARD;
extern const unsigned int NUM_OF_TREEBASED_CHILD_CLUSTERS;
extern const unsigned int FETCH_LOOKUP_MSG_MAX_RETRY;
extern const unsigned int MAX_CONTRACT_DEPTH;

extern const bool TEST_NET_MODE;
extern const bool EXCLUDE_PRIV_IP;
extern const bool ENABLE_DO_REJOIN;
extern const bool FULL_DATASET_MINE;
extern const bool OPENCL_GPU_MINE;
extern const bool CUDA_GPU_MINE;
extern const bool LOOKUP_NODE_MODE;
extern const bool USE_REMOTE_TXN_CREATOR;
extern const bool ARCHIVAL_NODE;
extern const bool BROADCAST_GOSSIP_MODE;
extern const bool GOSSIP_CUSTOM_ROUNDS_SETTINGS;
extern const bool BROADCAST_TREEBASED_CLUSTER_MODE;
extern const bool GET_INITIAL_DS_FROM_REPO;

extern const std::vector<std::string> GENESIS_WALLETS;
extern const std::vector<std::string> GENESIS_KEYS;

extern const std::string GPU_TO_USE;
extern const unsigned int OPENCL_LOCAL_WORK_SIZE;
extern const unsigned int OPENCL_GLOBAL_WORK_SIZE_MULTIPLIER;
extern const unsigned int OPENCL_START_EPOCH;
extern const unsigned int CUDA_BLOCK_SIZE;
extern const unsigned int CUDA_GRID_SIZE;
extern const unsigned int CUDA_STREAM_NUM;
extern const unsigned int CUDA_SCHEDULE_FLAG;

#endif  // __CONSTANTS_H__
