# -*- coding = utf-8 -*-
# @Time:2021-07-12 17:54
# @Author:Freak
# @File:access-contract.py
# @SoftWare:PyCharm
from web3 import Web3
import json


def load_via_artifact(w3, contract):
    fn_abi = './contract/build/{0}.abi'.format(contract)
    fn_addr = './contract/build/{0}.addr'.format(contract)

    with open(fn_abi) as f:
        abi = json.load(f)

    with open(fn_addr) as f:
        addr = f.read()

    return w3.eth.contract(address=addr, abi=abi)


w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
accounts = w3.eth.accounts

token = load_via_artifact(w3, 'EzToken')

balance = token.functions.balanceOf(accounts[0]).call()
print('token balance of account#0 => {0}'.format(balance))

txhash = token.functions.transfer(accounts[1], 10).transact({'from': accounts[0]})
receipt = w3.eth.waitForTransactionReceipt(txhash)
if (receipt is not None):
    print('token transfered from account#0 to account#1')
else:
    print('token transfer failed.')

balance = token.functions.balanceOf(accounts[1]).call()
print('token balance of account#1 => {0}'.format(balance))
