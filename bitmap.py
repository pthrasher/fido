#!/usr/bin/env python

import struct
import socket

class Bitmap(object):
    def __init__(self, host='0.0.0.0', port=8030):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port,))

    def get(self, offset):
        self.s.sendall(struct.pack('Ic', offset, 'r'))
        o, val = struct.unpack('Ic', self.s.recv(5))

        if val == 's':
            return True
        return False

    def set(self, offset, value):
        action = 's' if value == True else 'u'

        self.s.sendall(struct.pack('Ic', offset, action))
        o, val = struct.unpack('Ic', self.s.recv(5))

        if val == action:
            return True
        return False


class IPMap(object):
    def __init__(self):
        self.b = Bitmap()

    def ip2long(self, i):
        '''Convert IP Address into Long'''
        s = i.split('.')
        return (int(s[0]) << 24 | int(s[1]) << 16 | int(s[2]) << 8 | int(s[3]))

    def has_ip(self, ipaddr):
        ipl = self.ip2long(ipaddr)
        return self.b.get(ipl)

    def mark_ip(self, ipaddr, val):
        ipl = self.ip2long(ipaddr)
        self.b.set(ipl, val)


import sys
runs = 0
str_buffer = ""

host = '0.0.0.0'
port = 8030
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port,))

# = 300,000 iterations
for a in range(1, 1251):

    for i in range(1, 241):
        runs += 1
        str_buffer += struct.pack("Ic", i*a, 's')

    s.sendall(str_buffer)
    str_buffer = ""
    s.recv(1250)

s.close()

print "Made %d individual calls to the server" % runs



