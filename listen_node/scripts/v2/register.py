import time
from scripts.v4.Client.TCP_Client import TCP_Client
from File_oper import File
from scripts.v4.pack.BasePacker import BasePacker
from scripts.v4.SM.sm2 import SM2


tcp_cli = TCP_Client('8.130.14.55',8849)
sm2 = SM2()
if __name__ == '__main__':
    print "Welcome to ROSWithChain!"
    time.sleep(1)

    ip,port = tcp_cli.target_ip_port_detection()

    flag = File.isNull('./deploy_args/Username.txt')
    if flag == True:
        pass
    else:
        print '-------------Warning---------------\n' \
              'The name you enter is important, please keep that for future using!.\n'
        User_name = raw_input('Please input your username')
        passwd = raw_input('Please input your passwd')
        sm4_key = raw_input("For the first use, please input your sm4 key for encryption.")
        sm2_key = raw_input("Please enter the sm2 public key from the readme")
        sm4_enc = sm2.encrypt(sm4_key,sm2_key)
        while BasePacker.command_send('register',cli=tcp_cli,username=User_name,sm4_enc=sm4_enc,passwd=passwd):
            User_name = raw_input('Please choose your username again')
        name = File('./deploy_args/Username.txt')
        passwd_ = File('./deploy_args/passwd.txt')
        key_file = File('./deploy_args/keys.txt')
        name.write_txt(User_name)
        passwd_.write_txt(passwd)
        key_file.write_txt(sm4_key)

    tcp_cli.close()



