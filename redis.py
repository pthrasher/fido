#!/usr/bin/env python

import struct
import socket

def format_redis_msg(msg):
    cmds = msg.split(" ")
    buf = "*%d\r\n" % len(cmds)

    for cmd in cmds:
        buf += "$%d\r\n%s\r\n" % (len(cmd), cmd,)

    return buf



# Testing purposes
# for simple timing test:
#     `time python redis.py`
if __name__ == "__main__":
    runs = 0 # We're going to count how many queries we sent to the api.

    str_buffer = "" # empty buffer for what will be sent to the server.
    byte_count = 0

    host = 'localhost'
    port = 6379
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port,))

    for a in range(1, 450000+1): # send 1250 packets.
        runs += 1
        str_ = "*3\r\n$6\r\nGETBIT\r\n$4\r\nfido\r\n$%d\r\n%d\r\n" % (len(str(a)), a,)
        byte_count += len(str_)
        str_buffer += str_

        if byte_count >= 1200:
            s.sendall(str_buffer) # send a packet at a time. (1200ish bytes to be safe)
            str_buffer = "" # Empty out the buffer.
            byte_count = 0 # reset our byte count to 0
            s.recv()

    s.sendall(str_buffer) # send a packet at a time. (1200ish bytes to be safe)
    str_buffer = "" # Empty out the buffer.
    byte_count = 0 # reset our byte count to 0
    s.recv(1500)

    print "Made %d individual calls to the server" % runs


    s.close()
