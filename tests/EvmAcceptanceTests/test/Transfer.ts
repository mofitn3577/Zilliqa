import {expect} from "chai";
import {BigNumber} from "ethers";
import {ethers} from "hardhat";
import {parallelizer} from "../helpers";

const FUND = ethers.utils.parseUnits("1", "gwei");

describe("ForwardZil contract functionality", function () {
  before(async function () {
    this.contract = await parallelizer.deployContract("ForwardZil");
    this.signer = this.contract.signer;
  });

  it("Should return zero as the initial balance of the contract", async function () {
    expect(await ethers.provider.getBalance(this.contract.address)).to.be.eq(0);
  });

  it(`Should move ${ethers.utils.formatEther(FUND)} ethers to the contract if deposit is called`, async function () {
    expect(await this.contract.deposit({value: FUND})).changeEtherBalance(this.contract.address, FUND);
  });

  // TODO: Add notPayable contract function test.

  it("Should move 1 ether to the owner if withdraw function is called so 1 ether is left for the contract itself [@transactional]", async function () {
    expect(await this.contract.withdraw()).to.changeEtherBalances(
      [this.contract.address, this.signer.address],
      [ethers.utils.parseEther("-1.0"), ethers.utils.parseEther("1.0")],
      {includeFee: true}
    );
  });

  it("should be possible to transfer ethers to the contract", async function () {
    const prevBalance = await ethers.provider.getBalance(this.contract.address);
    await parallelizer.sendTransaction({
      to: this.contract.address,
      value: FUND
    });

    const currentBalance = await ethers.provider.getBalance(this.contract.address);
    expect(currentBalance.sub(prevBalance)).to.be.eq(FUND);
  });
});

describe("Transfer ethers", function () {
  it("should be possible to transfer ethers to a user account", async function () {
    const payee = ethers.Wallet.createRandom();

    await parallelizer.sendTransaction({
      to: payee.address,
      value: FUND
    });

    expect(await ethers.provider.getBalance(payee.address)).to.be.eq(FUND);
  });

  it("should be possible to batch transfer using a smart contract", async function () {
    const ACCOUNTS_COUNT = 3;
    const ACCOUNT_VALUE = 1_000_000;

    const accounts = Array.from({length: ACCOUNTS_COUNT}, (v, k) =>
      ethers.Wallet.createRandom().connect(ethers.provider)
    );

    const addresses = accounts.map((signer) => signer.address);

    await parallelizer.deployContract("BatchTransferCtor", addresses, ACCOUNT_VALUE, {
      value: ACCOUNTS_COUNT * ACCOUNT_VALUE
    });

    const balances = await Promise.all(accounts.map((account) => account.getBalance()));
    balances.forEach((el) => expect(el).to.be.eq(ACCOUNT_VALUE));
  });

  // FIXME: https://zilliqa-jira.atlassian.net/browse/ZIL-5082
  xit("should be possible to batch transfer using a smart contract with full precision", async function () {
    const ACCOUNTS_COUNT = 3;
    const ACCOUNT_VALUE = 1_234_567;

    const accounts = Array.from({length: ACCOUNTS_COUNT}, (v, k) =>
      ethers.Wallet.createRandom().connect(ethers.provider)
    );

    const addresses = accounts.map((signer) => signer.address);

    await parallelizer.deployContract("BatchTransferCtor", addresses, ACCOUNT_VALUE, {
      value: ACCOUNTS_COUNT * ACCOUNT_VALUE
    });

    const balances = await Promise.all(accounts.map((account) => account.getBalance()));
    balances.forEach((el) => expect(el).to.be.eq(ACCOUNT_VALUE));
  });

  it("probably should be possible to use sent funds of smart contract", async function () {
    const TRANSFER_VALUE = 1_000_000;

    // Create random account
    const rndAccount = ethers.Wallet.createRandom().connect(ethers.provider);
    const randomAccount = rndAccount.address;

    let initialBal = await ethers.provider.getBalance(randomAccount);
    expect(initialBal).to.be.eq(0);

    const [owner] = await ethers.getSigners();
    let InitialOwnerbal = await ethers.provider.getBalance(owner.address);

    // check enough funds + gas
    expect(InitialOwnerbal).to.be.at.least(TRANSFER_VALUE * 1.1);

    // Deploy the contract
    const singleTransfer = await parallelizer.deployContract("SingleTransfer");

    // call SC with a value to move funds across
    await singleTransfer.doTransfer(randomAccount, TRANSFER_VALUE, {
      gasLimit: 25000000,
      value: TRANSFER_VALUE
    });

    const receivedBal = await ethers.provider.getBalance(randomAccount);

    expect(receivedBal).to.be.eq(TRANSFER_VALUE);
  });

  it("should return check gas and funds consistency", async function () {
    let rndAccount = ethers.Wallet.createRandom();

    const FUND = BigNumber.from(100_000_000_000);

    await parallelizer.sendTransaction({
      to: rndAccount.address,
      value: FUND
    });

    rndAccount = rndAccount.connect(ethers.provider);

    const TRANSFER_VALUE = 100_000_000;

    // We can't use parallizer here since we need a hash of the receipt to inspect gas usage later
    const SingleTransferContract = await ethers.getContractFactory("SingleTransfer", rndAccount);
    const singleTransfer = await SingleTransferContract.deploy({value: TRANSFER_VALUE});
    await singleTransfer.deployed();

    async function getFee(hash: string) {
      const res = await ethers.provider.getTransactionReceipt(hash);
      const NORM_TXN_GAS = 50;
      const MIN_ETH_GAS = 21000;
      // Result should be scaled by (50/21000)
      return res.gasUsed.mul(res.effectiveGasPrice).mul(NORM_TXN_GAS).div(MIN_ETH_GAS);
    }

    const fee1 = await getFee(singleTransfer.deployTransaction.hash);

    const newBal = await ethers.provider.getBalance(rndAccount.address);

    const expectedNewBalance = FUND.sub(TRANSFER_VALUE).sub(fee1);

    expect(expectedNewBalance).to.be.eq(newBal);
  });
});
