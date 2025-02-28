/*
 * Copyright (C) 2019 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZILLIQA_SRC_LIBCONSENSUS_CONSENSUSLEADER_H_
#define ZILLIQA_SRC_LIBCONSENSUS_CONSENSUSLEADER_H_

#include <condition_variable>

#include "ConsensusCommon.h"

typedef std::function<bool(const zbytes& errorMsg, const Peer& from)>
    NodeCommitFailureHandlerFunc;
typedef std::function<bool(std::map<unsigned int, zbytes>)>
    ShardCommitFailureHandlerFunc;
typedef std::function<bool(zbytes& dst, unsigned int offset,
                           const uint32_t consensusID,
                           const uint64_t blockNumber, const zbytes& blockHash,
                           const uint16_t leaderID, const PairOfKey& leaderKey,
                           zbytes& messageToCosign)>
    AnnouncementGeneratorFunc;

/// Implements the functionality for the consensus committee leader.
class ConsensusLeader : public ConsensusCommon {
  enum Action {
    SEND_ANNOUNCEMENT = 0x00,
    PROCESS_COMMIT,
    PROCESS_RESPONSE,
    PROCESS_FINALCOMMIT,
    PROCESS_FINALRESPONSE,
    PROCESS_COMMITFAILURE
  };

  // Consensus session settings
  unsigned int m_numForConsensus;
  unsigned int m_numForConsensusFailure;

  // Received commits
  std::mutex m_mutex;
  std::atomic<unsigned int> m_commitCounter{0};

  std::mutex m_mutexAnnounceSubsetConsensus;
  std::condition_variable cv_scheduleSubsetConsensus;
  bool m_sufficientCommitsReceived;
  unsigned int m_sufficientCommitsNumForSubsets;

  std::vector<bool> m_commitMap;
  std::vector<std::vector<CommitPoint>>
      m_commitPointMap;  // ordered list of commits of size = committee size
  std::vector<std::vector<CommitPoint>>
      m_commitPoints;  // unordered list of commits of size
                       // = 2/3 of committee size + 1
  // Generated challenge
  Challenge m_challenge;

  unsigned int m_commitFailureCounter;
  std::map<unsigned int, zbytes> m_commitFailureMap;

  // Tracking data for each consensus subset
  struct ConsensusSubset {
    std::vector<bool> commitMap;
    std::vector<CommitPoint> commitPointMap;  // Ordered list of commits of
                                              // fixed size = committee size
    std::vector<CommitPoint> commitPoints;
    unsigned int responseCounter{};
    Challenge challenge;  // Challenge / Finalchallenge value generated
    std::vector<Response> responseDataMap;  // Ordered list of responses of
                                            // fixed size = committee size
    /// Response map for the generated collective signature
    std::vector<bool> responseMap;
    std::vector<Response> responseData;
    Signature collectiveSig;
    State state{};  // Subset consensus state
  };
  std::vector<ConsensusSubset> m_consensusSubsets;
  unsigned int m_numSubsetsRunning;

  NodeCommitFailureHandlerFunc m_nodeCommitFailureHandlerFunc;
  ShardCommitFailureHandlerFunc m_shardCommitFailureHandlerFunc;

  AnnouncementGeneratorFunc m_collSigAnnouncementGeneratorFunc;

  // Internal functions
  bool CheckState(Action action);
  bool CheckStateSubset(uint16_t subsetID, Action action);
  void SetStateSubset(uint16_t subsetID, State newState);
  void GenerateConsensusSubsets();
  bool StartConsensusSubsets();
  void SubsetEnded(uint16_t subsetID);
  bool ProcessMessageCommitCore(const zbytes& commit, unsigned int offset,
                                Action action,
                                ConsensusMessageType returnmsgtype,
                                State nextstate, const Peer& from);
  bool ProcessMessageCommit(const zbytes& commit, unsigned int offset,
                            const Peer& from);
  bool ProcessMessageCommitFailure(const zbytes& commitFailureMsg,
                                   unsigned int offset, const Peer& from);
  bool GenerateChallengeMessage(zbytes& challenge, unsigned int offset);
  bool ProcessMessageResponseCore(const zbytes& response, unsigned int offset,
                                  Action action,
                                  ConsensusMessageType returnmsgtype,
                                  State nextstate, const Peer& from);
  bool ProcessMessageResponse(const zbytes& response, unsigned int offset,
                              const Peer& from);
  bool GenerateCollectiveSigMessage(zbytes& collectivesig, unsigned int offset,
                                    uint16_t subsetID);
  bool ProcessMessageFinalCommit(const zbytes& finalcommit, unsigned int offset,
                                 const Peer& from);
  bool ProcessMessageFinalResponse(const zbytes& finalresponse,
                                   unsigned int offset, const Peer& from);

 public:
  /// Constructor.
  ConsensusLeader(
      uint32_t consensus_id,     // unique identifier for this consensus session
      uint64_t block_number,     // latest final block number
      const zbytes& block_hash,  // unique identifier for this consensus session
      uint16_t node_id,  // leader's identifier (= index in some ordered lookup
                         // table shared by all nodes)
      const PrivKey& privkey,        // leader's private key
      const DequeOfNode& committee,  // ordered lookup table of pubkeys for this
                                     // committee (includes leader)
      unsigned char class_byte,      // class byte representing Executable class
                                     // using this instance of ConsensusLeader
      unsigned char ins_byte,        // instruction byte representing consensus
                                     // messages for the Executable class
      NodeCommitFailureHandlerFunc nodeCommitFailureHandlerFunc,
      ShardCommitFailureHandlerFunc shardCommitFailureHandlerFunc,
      bool isDS = false);
  /// Destructor.
  ~ConsensusLeader();

  /// Triggers the start of consensus on a particular message (e.g., DS block).

  bool StartConsensus(
      const AnnouncementGeneratorFunc& announcementGeneratorFunc,
      const AnnouncementGeneratorFunc& newAnnouncementGeneratorFunc = nullptr,
      bool useGossipProto = false);

  /// Function to process any consensus message received.
  bool ProcessMessage(const zbytes& message, unsigned int offset,
                      const Peer& from);

  unsigned int GetNumForConsensusFailure() { return m_numForConsensusFailure; }

  /// Function to check for missing responses
  void Audit();

  /// Function to log the responses stats
  void LogResponsesStats(unsigned int subsetID);

 private:
  static std::map<Action, std::string> ActionStrings;
  std::string GetActionString(Action action) const;
};

#endif  // ZILLIQA_SRC_LIBCONSENSUS_CONSENSUSLEADER_H_
