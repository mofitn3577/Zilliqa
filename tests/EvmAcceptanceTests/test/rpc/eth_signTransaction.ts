import sendJsonRpcRequest from "../../helpers/JsonRpcHelper";
import {assert} from "chai";
import hre from "hardhat";
import logDebug from "../../helpers/DebugHelper";

const METHOD = "eth_signTransaction";

describe("Calling " + METHOD, function () {
  describe("When on Zilliqa network", function () {
    before(async function () {
      if (!hre.isZilliqaNetworkSelected()) {
        this.skip();
      }
    });

    it("should return an error on sending a sign transaction request", async function () {
      await sendJsonRpcRequest(
        METHOD,
        2,
        ["0xF0C05464f12cB2a011d21dE851108a090C95c755", "0xdeadbeaf"],
        (result, status) => {
          logDebug(result);

          assert.equal(status, 200, "has status code");
          assert.isNumber(result.error.code);
          assert.equal(Number(result.error.code), -32601);
          assert.isString(result.error.message, "is string");
          assert.equal(
            result.error.message,
            "METHOD_NOT_FOUND: The method being requested is not available on this server"
          );
        }
      );
    });
  });
});
