/*
 * Copyright (C) 2022 Zilliqa
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

#include "UnixDomainSocketClient.h"

#include "libUtils/Logger.h"

#include <jsonrpccpp/common/sharedconstants.h>
#include <boost/asio.hpp>

namespace rpc {

void UnixDomainSocketClient::SendRPCMessage(const std::string& message,
                                            std::string& result) {
  LOG_MARKER();
  try {
    using boost::asio::local::stream_protocol;
    boost::asio::io_context io_context;
    stream_protocol::socket unixDomainSocket(io_context);
    boost::asio::streambuf streamBuffer;
    //
    // Connect to the stream
    try {
      unixDomainSocket.connect(stream_protocol::endpoint(m_path));
    } catch (std::exception& e) {
      if (LOG_SC) {
        LOG_GENERAL(INFO, "Exception calling connect " << e.what());
      }
      throw;
    }
    std::string toSend = message + DEFAULT_DELIMITER_CHAR;

    // Write the JsonRpc
    try {
      boost::asio::write(unixDomainSocket,
                         boost::asio::buffer(toSend, toSend.length()));
    } catch (std::exception& e) {
      if (LOG_SC) {
        LOG_GENERAL(INFO, "Exception calling write " << e.what());
      }
      throw;
    }
    //
    // Read the Response
    try {
      boost::asio::read_until(unixDomainSocket, streamBuffer,
                              DEFAULT_DELIMITER_CHAR);
    } catch (std::exception& e) {
      if (LOG_SC) {
        LOG_GENERAL(INFO, "Exception calling read " << e.what());
      }
      throw;
    }
    std::istream is(&streamBuffer);
    // transfer it into the users object
    std::getline(is, result);
  } catch (std::exception& e) {
    LOG_GENERAL(WARNING,
                "Exception caught in custom SendRPCMessage " << e.what());
  }
}

}  // namespace rpc
