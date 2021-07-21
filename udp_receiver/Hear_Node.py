from socket import *
from web3 import Web3
import json

def load_via_artifact(w3, contract):
    fn_abi = './{0}.abi'.format(contract)
    fn_addr = './{0}.addr'.format(contract)

    with open(fn_abi) as f:
        abi = json.load(f)

    with open(fn_addr) as f:
        addr = f.read()

    return w3.eth.contract(address=addr, abi=abi)

def Recv():
    Udp_Socket = socket(AF_INET,SOCK_DGRAM)
    Udp_Socket.bind(("192.168.1.102",8848))
    w3 = Web3(Web3.HTTPProvider('http://localhost:8545'))
    accounts = w3.eth.accounts
    token = load_via_artifact(w3, 'Robotchain')
    token.functions.registerRobot(1).transact({'from': accounts[0]})
    while 1:
        Recv_Data = Udp_Socket.recvfrom(1024)
        # Recv_Msg = Recv_Data[0]
        # Sender_Addr = Recv_Data[1]
        # print("%s:%s"%(str(Sender_Addr),Recv_Msg.decode('utf-8')))
        Recv_msg = Recv_Data[0].decode('utf-8')
        Recv_msg = Recv_msg.split("+")

        tar_x = int(Recv_msg[0])
        tar_y = int(Recv_msg[1])
        print(tar_x)
        print(tar_y)

        token.functions.setPosition(tar_x,tar_y).transact({'from': accounts[0]})
        token.functions.setVel(0).transact({'from': accounts[0]})

        (tx,ty) = token.functions.getOwnPosition().call({'from': accounts[0]})
        print(tx)
        print(ty)
        # print (Recv_msg)
        # while 1:
        #     print(Recv_msg)
if  __name__ == "__main__":
    Recv()