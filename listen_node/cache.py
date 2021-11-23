from scripts.v4.circle_queue import queue
class Cache(object):
    __slots__ = ('_num','_cache')
    def __init__(self,num):
        self._num = num
        self._cache = []
        for i in range(self._num):
            self._cache[i] = queue(1024*100)

    @property
    def num(self):
        return self._num
    @num.setter
    def num(self,num):
        self._num = num

    def append(self):
        self._cache.append(queue(1024*10))