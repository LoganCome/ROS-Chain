# -*- coding = utf-8 -*-
# @Time:2021-07-12 17:57
# @Author:Freak
# @File:deploy-contract.py
# @SoftWare:PyCharm
from web3 import Web3
from web3.utils.encoding import to_hex
import json, sys

w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
accounts = w3.eth.accounts

artifact = 'EzToken'

fn_abi = './contract/build/{0}.abi'.format(artifact)
fn_bin = './contract/build/{0}.bin'.format(artifact)
fn_addr = './contract/build/{0}.addr'.format(artifact)

with open(fn_abi,'r') as f:
  abi = json.load(f)

with open(fn_bin,'r') as f:
  bin = f.read()

factory = w3.eth.contract(abi=abi,bytecode=bin)

wrapper = factory.constructor(1000000000,'HAPPY COIN',0,'HAPY')
#print(wrapper.data_in_transaction)

tx_hash = wrapper.transact({'from':accounts[0]})
print('deploy tx hash => {0}'.format(to_hex(tx_hash)))

receipt = w3.eth.waitForTransactionReceipt(tx_hash)
print('deployed adress => {0}'.format(receipt.contractAddress))

with open(fn_addr,'w') as f:
  f.write(receipt.contractAddress)
print('contract address saved => {0}'.format(fn_addr))
