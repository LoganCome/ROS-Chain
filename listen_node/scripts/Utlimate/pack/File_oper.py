'''
文件操作封装：
1.判断是否为空
2.读取
3.写入
'''
class File(object):
    def __init__(self,names):
        self.names = names
        self.lines = []

    @staticmethod
    def isNull(names):
        try:
            with open(names,'r') as f:
                context = f.readlines()
        except IOError:
            print ("No such file! Please check!")
        if context == None or context == '\n' or context == ' ':
            print (context+'sd')
            return False
        else:
            return True

    def read_txt(self):
        try:
            with open(self.names,'r') as f:
                context = f.readlines()
        except IOError:
            print ("No such file! Please check!")
            exit(1)
        for line in context:
            line =line.strip('\n')
            self.lines.append(line)
        return self.lines

    def write_txt(self,context):
        with open(self.names,'w') as f:
            context = context.split(' ')
            for i in context:
                if i != '\n':
                    f.write(i)
                    f.write('\n')
                else:
                    pass


