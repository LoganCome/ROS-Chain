import multiprocessing
import json
import os
from scripts.v4.pack.deploys import All_DeployToSend,Odomtery_DeployToSend,Image_DeployToSend,Imu_DeployToSend,CompressedImage_DeployToSend
from scripts.v4.File_oper import File

class BasePacker(object):

    def __init__(self,key,port,username):
       self._key = key
       self._sendingport = port
       self._user = username


    def message_send(self,reids,lock,cli):
        '''

        :param cache:
        :param lock:
        :param cli:
        :return:
        '''
        pass

    """
    commands: register / user_acc / verification / Monitor_start / Send / OnChain / Authority_Grant / Info_Check / topics_reset / None
    """
    @staticmethod
    def command_send(command,**kwargs):
       if command == 'register':
           try:
               username = kwargs.get('username')
               passwd = kwargs.get('passwd')
               sm4_enc = kwargs.get('SM4')
               cli = kwargs.get('cli')
               msg1 = json.dumps({'username':username,'command':'register'})
               cli.send_msg(msg1)
               recv = cli.recv_msg()
               if recv == 'Acc':
                   msg = json.dumps({'username':username,'passwd':passwd,'sm4':sm4_enc,'command':'user_acc'})
                   cli.send_msg(msg)
                   recv_2 = cli.recv_msg()
                   if recv_2 == 'Register successfully!':
                       print 'Register successfully!'
                       return False
               else:
                   print 'Duplicate name! Try another name.'
                   return True
           except Exception:
               print 'Bad args! Please check!'
               exit(1)

       elif command == 'verification':
           try:
               username = kwargs.get('username')
               passwd = kwargs.get('passwd')
               cli = kwargs.get('cli')

               msg_sending = json.dumps({'username':username,'passwd':passwd,'command':'verification'})
               cli.send_msg(msg_sending)
               recv = cli.recv_msg()
               if recv == 'Acc':
                   print 'identity verified.'
                   return True
               else:
                   print 'verification failure!'
                   return False
           except Exception:
               print 'Bad args! Please check!'
               exit(1)
       elif command == 'Monitor_start':
           try:
               os.popen('./monitor.sh')
           except Exception:
               print 'Script file execution failed!'\
                     'Please give corresponding file execution permission!'
               exit(1)
       elif command == 'Send':
           try:
               all = kwargs.get('all')
               key = File('./deploy_args/keys.txt').read_txt()
               if all == True:
                   All_DeployToSend(kwargs.get('redis'),kwargs.get('cli'),kwargs.get('username'),key)
               else:
                   type_ = kwargs.get('type')
                   if type_ == 'Odometry':
                       Odomtery_DeployToSend(kwargs.get('redis'),kwargs.get('cli'),kwargs.get('username'),key)
                   elif type_ == 'Image':
                       Image_DeployToSend(kwargs.get('redis'),kwargs.get('cli'),kwargs.get('username'),key)
                   elif type_ == 'Imu':
                       Imu_DeployToSend(kwargs.get('redis'),kwargs.get('cli'),kwargs.get('username'),key)
                   elif type_ == 'CompressedImage':
                       CompressedImage_DeployToSend(kwargs.get('redis'),kwargs.get('cli'),kwargs.get('username'),key)
           except Exception:
               print 'Bad arguments!'
               exit(1)

       elif command == 'OnChain':
           pass
       elif command == 'Info_Check':
           pass
       elif command == 'Authority_Grant':
           pass
       elif command == 'topics_reset':
           pass

