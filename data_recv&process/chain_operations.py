'''
区块链操作封装
'''
import time

import requests
import web3
from web3 import Web3
import json
from solcx import compile_source
from solcx import set_solc_version
from socket import *
import random
import multiprocessing
import sm4

"""
基于以太坊的无人车只能调度模型
上链操作管理模块

本项目由海南大学机器人与人工智能协会区块链小组成员共同维护
"""

URI = '127.0.0.1'
port1 = 9092
port2 = 8850
port3 = 9999
first_load = True
threads = []


class MESG_Send(object):
    def __init__(self, ip, port):
        local_addr = (ip, port1)
        self.ip = ip
        self.port = port
        self.udp_socket = socket(AF_INET, SOCK_DGRAM)
        self.udp_socket.bind(local_addr)
        self.dest_addr = (ip, port)

    def send_msg(self, msg):
        send_data = msg
        self.udp_socket.sendto(send_data.encode('utf-8'), self.dest_addr)
        self.udp_socket.close()


def Ethereum_info_get():
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    # 加载合约账户
    accounts = w3.eth.accounts
    # 加载合约
    token = load_via_artifact(w3)
    return token, accounts


# 根据本地链码的二进制文件以及以太坊上的链码部署地址初始化合约
def load_via_artifact(w3):
    set_solc_version('0.5.16')
    complied_sol = compile_source(
        '''

    //SPDX-License-Identifier: <SPDX-License>
    pragma solidity >0.5.0;
    pragma experimental ABIEncoderV2;


    contract RobotChainContract {

        uint256 public ROSCount;

        address payable Administrator;

        struct ROS_Entity{
            address payable rosaddress;
            bool isRegistered;
            uint256 enode;
            bytes token;
        }

        bytes[][200] public upload_time;
        bytes[][200] public ciphertext;
        bytes[][200] public token_List;
        bytes[][200] public t_ciphertext;
        address payable[] public User_addresses;

        mapping(address => ROS_Entity) public Users;


        constructor() public {
            ROSCount = 0;
            Administrator = msg.sender;

            Users[Administrator].isRegistered = true;
            Users[Administrator].enode = 0;
            // Users[Administrator].token = token;
        }

        function register(bytes memory token) public returns (uint){
            require(
                msg.sender != Administrator,
                "The Administrator don't need to register!");
            require(
                Users[msg.sender].isRegistered != true,
                "You have registered!");
            require(ROSCount < 200,"Maximum limit exceeded!");

            if(ROSCount < 200){    
                ROSCount += 1;
                Users[msg.sender].isRegistered = true;
                Users[msg.sender].enode = ROSCount;
                Users[msg.sender].token = token;
                token_List[ROSCount].push(token);
                User_addresses.push(msg.sender);
                return ROSCount;
            }else{
                return 0;

            }
        }

        function utilCompareInternal(bytes memory a, bytes memory b) internal pure returns (bool) {

            if (a.length != b.length) {
                return false;
            }

            for (uint i = 0; i < a.length; i ++) {
                if(a[i] != b[i]) {
                    return false;
                }
            }

            return true;
        }

        function upload_data(bytes memory ctext,bytes memory token,bytes memory utime) public{

            require(Users[msg.sender].isRegistered == true,
            "U can't upload data cause u have't register! If u want to use the function of ROS2Chain, please register firstly");

            require(utilCompareInternal(Users[msg.sender].token,token) == true,
            "Wrong token!");

            uint256 index = uint256(Users[msg.sender].enode);
            // bytes memory text = bytes(ctext);
            // bytes memory time = bytes(utime);
            ciphertext[index].push(ctext);
            upload_time[index].push(utime);
        }

        function grant_authority(address to) public{
            require(Users[msg.sender].isRegistered == true,
            "U can't grant cause u have't register! If u want to use the function of ROS-Ethereum, please register firstly");
            require(Users[to].isRegistered == true,
            "The user u want to grant have't registered!");

            bytes memory t = Users[msg.sender].token;

            uint256 index = uint256(Users[to].enode);
            token_List[index].push(t);


        }

        function eth_apply_for() public returns (bool){
            require(Users[msg.sender].isRegistered == true,
            "U can't apply for ether cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
            // address address_apply_for_eth = msg.sender;
            // require(address_apply_for_eth.balance < 10,
            // "U have some eth, reject to didisrstribute!");

            msg.sender.transfer(10);
            return true;
        }

        function isExistToken(address from,bytes memory token) public view returns (bool){

            bytes memory temp_token = token;

            for(uint i = 0; i < User_addresses.length; i++ ){
                if(from == User_addresses[i]){
                    for(uint j = 0; j < token_List[i+1].length; j++){
                        if(token_List[i+1][j].length == temp_token.length){
                            for(uint k = 0; k < token_List[i+1][j].length+1; k++){
                                if(k == temp_token.length){
                                    return true;
                                }
                                if(token_List[i+1][j][k] == temp_token[k]){
                                    continue;
                                }else{
                                    break;
                                }
                            }
                        }else{
                            continue;
                        }
                    }
                }else{
                    continue;
                }
            }
            return false;
        }

        function data_query_by_time_stamp(bytes memory token,bytes memory utime) public view returns (bytes memory){
            require(Users[msg.sender].isRegistered == true,
            "U can't query data cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
            require(isExistToken(msg.sender,token) == true,
            "Token dosen't exist!");

            bytes memory temp_time = utime;
            bytes memory result;
            result = bytes('0x00');

            for(uint i = 0; i < User_addresses.length; i++){
                if (utilCompareInternal(Users[User_addresses[i]].token,token) == true){
                    for(uint j = 0; j < upload_time[i+1].length; j++){
                        if(upload_time[i+1][j].length == temp_time.length){
                            for(uint k =0; k < temp_time.length+1; k++){
                                if(k == temp_time.length){
                                    // return (ciphertext[i+1][j]);
                                    result = ciphertext[i+1][j];
                                }
                                if(upload_time[i+1][j][k] == temp_time[k]){
                                    continue;
                                }else{
                                    break;
                                }

                            }
                        }else{
                            continue;
                        }
                    }
                }else{
                        continue;
                    }
            }
        return result;
        }

         function data_query_all(bytes memory token, address from )public view returns (bytes[] memory){
            require(Users[msg.sender].isRegistered == true,
            "U can't query data cause u have't register! If u want to use the function of ROS2Chain, please register firstly");
            require(isExistToken(msg.sender,token) == true,
            "Token dosen't exist!");
            require(Users[from].isRegistered==true,
            "The address u want to check dosen't register!");
            require(isExistToken(from,token) == true,
            "Token dosen't match!");

            return ciphertext[Users[from].enode];


        }

    }

        '''
    )

    contract_id, contract_interface = complied_sol.popitem()
    abi = contract_interface['abi']
    tx_receipt = w3.eth.wait_for_transaction_receipt(
        b'C\xa1\xb0z\xeb\tp\x8e\x80\x98\xe2_*:\xbe\x8fm\xc6\xf9\xcc\xaf\xe3\xa6\xc6\xd0F\xd8\xb1\xa1N@b'
    )
    greeter = w3.eth.contract(address=tx_receipt.contractAddress, abi=abi)
    return greeter

def register(token, accounts, num, passwd):
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    w3.geth.personal.unlock_account(accounts[num], str(num))
    try:
        receipt = token.functions.register(Web3.toBytes(text=passwd)).transact({'from': accounts[num]})
    except requests.ConnectionError:
        print("Errors in Chain.")
        return False
    except web3.exceptions:
        print("You have registered! or insufficient funds, please check.")
        return False
    if w3.eth.wait_for_transaction_receipt(receipt).blockHash == '':
        return False
    else:

        return num


def data_on_chain_cop(token, accounts, cpi, num, passwd):
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    w3.geth.personal.unlock_account(accounts[num], str(num))
    stmp = time.time()
    date = time.localtime(stmp)
    format_time = time.strftime('%Y-%m-%d %H:%M:%S', date)
    try:
        receipt = token.functions.upload_data(Web3.toBytes(text=str(cpi)),Web3.toBytes(text=str(passwd)),Web3.toBytes(text=str(format_time))).transact({'from': accounts[num]})
    except requests.ConnectionError:
        print("Errors in Chain.")
        return False
    except web3.exceptions:
        print("You have registered! or insufficient funds, please check.")
        return False
    if w3.eth.wait_for_transaction_receipt(receipt).blockHash == '':
        return False
    else:
        return True


def Info_Check(token, accounts, num, passwd, address_num):
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    w3.geth.personal.unlock_account(accounts[num], str(num))
    w3.geth.personal.unlock_account(accounts[address_num], str(address_num))
    return token.functions.data_query_all(Web3.toBytes(text=passwd), accounts[address_num]).call({'from': accounts[num]})

def Authority_Grant(token, accounts, num, address_num):
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    w3.geth.personal.unlock_account(accounts[num], str(num))
    w3.geth.personal.unlock_account(accounts[address_num], str(address_num))
    print(address_num)
    try:
        receipt = token.functions.grant_authority(accounts[address_num]).transact({'from': accounts[num]})
    except requests.ConnectionError:
        print("Errors in Chain.")
        return False
    except web3.exceptions:
        print("You have registered! or insufficient funds, please check.")
        return False
    if w3.eth.wait_for_transaction_receipt(receipt).blockHash == '':
        return False
    else:
        return True

