#! /usr/bin/env python
# coding: utf-8
class queue:
    def __init__(self, capacity = 10):
        self.capacity = capacity
        self.size = 0
        self.front = 0
        self.rear = 0
        self.array = [0]*capacity

    def is_empty(self):
        return 0 == self.size

    def is_full(self):
        return self.size == self.capacity

    def enqueue(self, element):
        if self.is_full():
            raise Exception('queue is full')
        self.array[self.rear] = element
        self.size += 1
        self.rear = (self.rear + 1) % self.capacity

    def dequeue(self):
        if self.is_empty():
            raise Exception('queue is empty')
        self.size -= 1
        self.front = (self.front + 1) % self.capacity

    def get_front(self):
        return self.array[self.front]

    def get_rear(self):
        return self.array[self.rear]

    def get_front_second(self):
        return self.array[((self.front + 1) % self.capacity)]

    def get_queue_length(self):
        if self.rear - self.front != 0:
            return (self.rear - self.front + self.capacity) % self.capacity
        else:
            return self.capacity

    def show_queue(self):
        for i in range(self.capacity):
            print self.array[i],
        print(' ')

    def get_queue_latest(self):
        return self.array[(self.rear-1+self.capacity) % self.capacity]

# if __name__ == '__main__':
#     txt = 'menghaoshi'
#     queue = queue()
#     for i in range(13):
#         if queue.is_full():
#             queue.dequeue()
#             queue.enqueue(txt+str(i))
#         else:
#             queue.enqueue(txt+str(i))
#     print queue.show_queue()
#     print queue.get_queue_length()
#     print str(queue.get_queue_latest())