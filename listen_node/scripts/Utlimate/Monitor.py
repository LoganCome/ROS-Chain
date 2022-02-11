'''
节点监听启动模块（需配合bash文件使用）
'''
from File_oper import File
from scripts.history.v4.Nodes.BaseNode import BaseNode

topic_names = []
Types = []
node_names = 'ROS-Chain_MonitorNode'

#变量初始化
def Variable_Init():
    global topic_names
    global Types

    topic_names = File('./deploy_args/topic_names.txt').read_txt()
    Types = File('./deploy_args/Types.txt').read_txt()


if __name__ == '__main__':

    Variable_Init()
    #节点实例化
    node = BaseNode(topic_names,Types,node_names)
    print "Welcome to the ROS-Chain Monitor Terminal." \
          "The process of monitoring is going to start."\
          "If you want to stop, just close the terminal."
    #开启节点监听
    node.monitor()



