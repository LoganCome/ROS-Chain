import subprocess

from File_oper import File

if __name__ == '__main__':
    subp = subprocess.Popen('python console.py',shell=True)
    subp.wait()