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

#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/unixdomainsocketclient.h>
#include <thread>
#include "common/Constants.h"
#include "libPersistence/ScillaMessage.pb.h"
#include "libServer/ScillaIPCServer.h"
#include "libUtils/Logger.h"

#define BOOST_TEST_MODULE scillaipc
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace jsonrpc;

BOOST_AUTO_TEST_SUITE(scillaipc)

// NOTE: Remember to use unique field names for different tests
//       since the data in the storage persists across tests.

// Connection sanity test.
BOOST_AUTO_TEST_CASE(test_connection) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: initialized server.");
  server.StartListening();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: server is now listening.");
  server.StopListening();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: server has stopped listening.");
}

// Simple non-map query.
BOOST_AUTO_TEST_CASE(test_query_simple) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a query to "set field foo with value".
  ProtoScillaQuery query;
  query.set_name("foo_test_query_simple");
  query.set_mapdepth(0);
  // Prepare the value itself to be set.
  ProtoScillaVal value;
  value.set_bval("420");  // The actual content doesn't matter.
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the value.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's now fetch that value back. Query remains same except that we have no
  // value now.
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  value.ParseFromString(result[1].asString());
  // Check that the fetched value is same as what we updated earlier.
  BOOST_CHECK_EQUAL(value.bval(), "420");

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

// Simple map query.
BOOST_AUTO_TEST_CASE(test_query_map_1) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a map key insertion query.
  ProtoScillaQuery query;
  query.set_name("foo_test_query_map_1");  // A map named "foo".
  query.set_mapdepth(1);                   // A single nested map.
  query.add_indices("key1");               // "key1" to be inserted.
  // Prepare the value itself to be set for "key1".
  ProtoScillaVal value;
  value.set_bval("420");  // The actual content doesn't matter.
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's now fetch that value back. Query remains same except that we have no
  // value now.
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  value.ParseFromString(result[1].asString());
  // Check that the fetched value is same as what we updated earlier.
  BOOST_CHECK_EQUAL(value.bval(), "420");

  // Let's fetch "key2" and ensure that it's not found.
  query.clear_indices();
  query.add_indices("key2");
  params["query"] = query.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  BOOST_CHECK_EQUAL(result[0].asBool(), false);

  // Delete "key1"
  query.clear_indices();
  query.add_indices("key1");
  query.set_ignoreval(true);
  params["query"] = query.SerializeAsString();
  params["value"] = "";
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's search for "key1" and ensure that it's not found.
  // query and params are the same as the previousl query.
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  BOOST_CHECK_EQUAL(result[0].asBool(), false);

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

// Nested map queries.
BOOST_AUTO_TEST_CASE(test_query_map_2) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a map key insertion query.
  ProtoScillaQuery query;
  query.set_name("foo_test_query_map2");  // A map named "foo".
  query.set_mapdepth(2);                  // A doubly nested map.

  // Add indices to the query.
  query.add_indices("key1a");
  query.add_indices("key2a");  // "[key1a][key2a]" to be inserted.
  // Prepare the value itself to be set for "[key1a][key2a]".
  ProtoScillaVal value;
  value.set_bval("420");  // The actual content doesn't matter.
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's now fetch that value back. Query remains same except that we have no
  // value now.
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  value.ParseFromString(result[1].asString());
  // Check that the fetched value is same as what we updated earlier.
  BOOST_CHECK_EQUAL(value.bval(), "420");

  // We'll now add an entire map as value.
  auto mval = unique_ptr<ProtoScillaVal_Map>(new ProtoScillaVal_Map());
  mval->mutable_m()->operator[]("key2b").set_bval("840");
  mval->mutable_m()->operator[]("key2c").set_bval("841");
  query.clear_indices();
  query.add_indices("key1b");
  value.Clear();
  value.set_allocated_mval(
      mval.release());  // Transfer ownership of mval to value.
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the mapval into "key1b".
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // We now expect the storage to contain:
  // foo[key1a][key2a] : 420
  // foo[key1b][key2b] : 840
  // foo[key1b][key2c] : 841
  query.clear_indices();  // Let's fetch back the entire map.
  params.clear();
  params["query"] = query.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);

  BOOST_CHECK_EQUAL(value.mval().m().size(), 2);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().at("key2a").bval(),
                    "420");
  BOOST_CHECK_EQUAL(value.mval().m().at("key1b").mval().m().size(), 2);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1b").mval().m().at("key2b").bval(),
                    "840");
  BOOST_CHECK_EQUAL(value.mval().m().at("key1b").mval().m().at("key2c").bval(),
                    "841");

  // Let's delete key1b.
  query.clear_indices();
  query.add_indices("key1b");
  query.set_ignoreval(true);
  params["query"] = query.SerializeAsString();
  params["value"] = "";
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // We now expect the storage to contain:
  // foo[key1a][key2a] : 420
  query.clear_indices();  // Let's fetch back the entire map.
  query.set_ignoreval(false);
  params.clear();
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());
  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);

  BOOST_CHECK_EQUAL(value.mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().at("key2a").bval(),
                    "420");

  // Add foo[key1b][key2c] back again, with a different value.
  query.add_indices("key1b");
  query.add_indices("key2c");
  value.set_bval("121");
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // We now expect the storage to contain:
  // foo[key1a][key2a] : 420
  // foo[key1b][key2c] : 121
  query.clear_indices();  // Let's fetch back the entire map.
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  // Ensure contents of map is correct.
  BOOST_CHECK_EQUAL(value.mval().m().size(), 2);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().at("key2a").bval(),
                    "420");
  BOOST_CHECK_EQUAL(value.mval().m().at("key1b").mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1b").mval().m().at("key2c").bval(),
                    "121");

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

// Add an empty map, and then replace it with a non-empty map.
BOOST_AUTO_TEST_CASE(test_query_empty_map) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a map key insertion query.
  ProtoScillaQuery query;
  query.set_name("foo_test_query_empty_map");  // A map named "foo".
  query.set_mapdepth(1);                       // A doubly nested map.

  ProtoScillaVal value;
  // Create an empty protobuf map.
  value.mutable_mval()->mutable_m();
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's try fetching back the empty map.
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().size(), 0);

  // Let's now insert foo[key1a] = "420"
  value.Clear();
  value.mutable_mval()->mutable_m()->operator[]("key1a").set_bval("420");
  params["value"] = value.SerializeAsString();
  params["query"] = query.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's try fetching back the map.
  // foo[key1a] = "420"
  query.clear_indices();
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").bval(), "420");

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

// Delete key in a map to make it empty and then query the map.
BOOST_AUTO_TEST_CASE(test_query_delete_to_empty) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a map key insertion query.
  ProtoScillaQuery query;
  query.set_name("foo_test_query_delete_to_empty");  // A map named "foo".
  query.set_mapdepth(1);                       // A doubly nested map.

  ProtoScillaVal value;
  // Create a map with single entry.
  value.mutable_mval()->mutable_m()->operator[]("key1").set_bval("420");
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's now delete that key.
  query.add_indices("key1");
  query.set_ignoreval(true);
  params["query"] = query.SerializeAsString();
  params["value"] = ""; // Dummy value
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let us now fetch the full map.
  query.clear_indices();
  query.set_ignoreval(false);
  params["query"] = query.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().size(), 0);

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

// Tests updating empty nested maps.
BOOST_AUTO_TEST_CASE(test_query_empty_map_2) {
  INIT_STDOUT_LOGGER();
  UnixDomainSocketServer s(SCILLA_IPC_SOCKET_PATH);
  ScillaIPCServer server(s);
  UnixDomainSocketClient c(SCILLA_IPC_SOCKET_PATH);
  Client client(c);

  server.StartListening();

  // Prepare a map key insertion query.
  ProtoScillaQuery query;
  query.set_name("foo_test_query_empty_map_2");  // A map named "foo".
  query.set_mapdepth(2);                       // A doubly nested map.

  ProtoScillaVal value;
  // Create an empty protobuf map.
  value.mutable_mval()->mutable_m();
  // Prepare JSON for JSON-RPC call.
  Json::Value params;
  params["query"] = query.SerializeAsString();
  params["value"] = value.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  Json::Value result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's try fetching back the empty map.
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().size(), 0);

  // Let's now insert foo[key1a]
  query.add_indices("key1a");
  value.Clear();
  value.mutable_mval()->mutable_m();
  params["value"] = value.SerializeAsString();
  params["query"] = query.SerializeAsString();
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());

  // Call the server method to add the key/val pair.
  result = client.CallMethod("updateStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Let's try fetching back the map.
  // foo[key1a] = []
  query.clear_indices();
  params["query"] = query.SerializeAsString();
  params.removeMember("value");
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Calling with JSON" +
                        params.toStyledString());
  result = client.CallMethod("fetchStateValue", params);
  LOG_GENERAL(INFO, "Test_ScillaIPCServer: Server returned JSON" +
                        result.toStyledString());

  // Parse the fetched result and assert.
  value.Clear();
  BOOST_CHECK_EQUAL(result[0].asBool(), true);
  // Compare the entries.
  value.ParseFromString(result[1].asString());
  BOOST_CHECK_EQUAL(value.has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().size(), 1);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").has_mval(), true);
  BOOST_CHECK_EQUAL(value.mval().m().at("key1a").mval().m().size(), 0);

  server.StopListening();
  LOG_GENERAL(INFO, "Test ScillaIPCServer test query done!");
}

BOOST_AUTO_TEST_SUITE_END()