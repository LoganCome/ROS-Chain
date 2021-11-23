from scripts.v4.circle_queue import queue
from threading import Thread
cache = queue(1024*100)

def write(cache,i):
    while True:
        cache.enqueue(i)
        print i


if __name__ == '__main__':
    pool=[None]*4
    for i in range(4):
        pool[i] = Thread(target=write,args=(cache,i,))
    for i in range(4):
        pool[i].start()
    for i in range(4):
        pool[i].join()