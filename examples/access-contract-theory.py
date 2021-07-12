# -*- coding = utf-8 -*-
# @Time:2021-07-12 17:50
# @Author:Freak
# @File:access-contract-theory.py
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
fn_addr = './contract/build/{0}.addr'.format(artifact)

with open(fn_abi,'r') as f:
  abi = json.load(f)

with open(fn_addr,'r') as f:
  addr = f.read()

def rpc_balanceOf(address):
  func_hash = keccak(text='balanceOf(address)')
  selector = func_hash[:4]

  encoded_params = encode_abi(['address'],[address])
  tx_data = to_hex(HexBytes(selector) + encoded_params)

  payload = {
    'to': addr,
    'data': tx_data
  }
  ret = w3.eth.call(payload)
  return decode_single('uint256',ret)

def rpc_transfer(owner,to,value):
  func_hash = keccak(text='transfer(address,uint256)')
  selector = func_hash[:4]

  encoded_params = encode_abi(['address','uint256'],[to,value])
  tx_data = to_hex(HexBytes(selector) + encoded_params)

  payload = {
    'from': owner,
    'to': addr,
    'data': tx_data
  }
  tx_hash = w3.eth.sendTransaction(payload)
  return w3.eth.waitForTransactionReceipt(tx_hash)


balance = rpc_balanceOf(accounts[0])
print('token balance before tx => {0}'.format(balance))

receipt = rpc_transfer(accounts[0],accounts[1],1)
if receipt is not None:
  print('token transfered.')

balance = rpc_balanceOf(accounts[0])
print('token balance after tx => {0}'.format(balance))
