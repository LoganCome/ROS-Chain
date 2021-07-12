# -*- coding = utf-8 -*-
# @Time:2021-07-12 17:57
# @Author:Freak
# @File:deploy-contract-theory.py
# @SoftWare:PyCharm
from web3 import Web3
from eth_utils.curried import keccak
from eth_abi import ( encode_abi,decode_single)
from hexbytes import HexBytes
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

encoded_params = encode_abi(['uint256','string','uint8','string'],[1000000000,'HAPPY COIN',0,'HAPY'])
tx_data = to_hex(HexBytes(bin) + encoded_params)

payload = {
  'from': accounts[0],
  'data': tx_data
}
tx_hash = w3.eth.sendTransaction(payload)
receipt = w3.eth.waitForTransactionReceipt(tx_hash)
print('deployed address => {0}'.format(receipt.contractAddress))

with open(fn_addr,'w') as f:
  f.write(receipt.contractAddress)

print('deployed address saved => {0}'.format(fn_addr))

