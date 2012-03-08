#!/usr/bin/env python

import struct
import socket

class FidoAPI(object):
    """
    API wrapper for the fido protocol.

    The fido protocol is super simple. It expects a struct containing the
    offset you're interested in as a 32 bit unsigned int, and an action as a
    char where 'r' means read, 's' means set, and 'u' means unset.
    """
    def __init__(self, api_host='0.0.0.0', api_port=8030):
        """
        Sets up the socket, and connects.
        """
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port,))

    def get(self, offset):
        """
        Creates a struct to send to the API, passing in the offset you'd like
        to check, as well as the char command 'r' for 'read'
        """
        self.s.sendall(struct.pack('Ic', offset, 'r'))
        o, val = struct.unpack('Ic', self.s.recv(5))

        if val == 's' and o == offset:
            return True
        return False

    def set(self, offset, value):
        """
        Creates a struct to send to the API, passing in the offset you'd like
        to change, and if you'd like to set the bit, you pass in True and False
        if you'd like to unset the bit. The protocol action for set is 's' and
        for unset it's 'u'.
        """
        action = 's' if value == True else 'u'

        self.s.sendall(struct.pack('Ic', offset, action))
        o, val = struct.unpack('Ic', self.s.recv(5))

        if val == action and o == offset:
            return True
        return False


class IPMap(object):
    def __init__(self):
        self.b = FidoAPI()

    @staticmethod
    def ip2long(ip):
        """
        Convert IP Address into Long
        """
        octet1, octet2, octet3, octet4 = ip.split('.')
        return (octet1 << 24 | octet2 << 16 | octet3 << 8 | octet4)

    def has_ip(self, ipaddr):
        ipl = self.ip2long(ipaddr)
        return self.b.get(ipl)

    def mark_ip(self, ipaddr, val):
        ipl = self.ip2long(ipaddr)
        self.b.set(ipl, val)


# Testing purposes
# for simple timing test:
#     `time python fido.py`
if __name__ == "__main__":
    runs = 0 # We're going to count how many queries we sent to the api.

    str_buffer = "" # empty buffer for what will be sent to the server.
    byte_count = 0

    host = '0.0.0.0'
    port = 8030
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port,))

    for a in range(1, 450000+1): # send 1250 packets.
        runs += 1
        str_buffer += struct.pack('Ic', a, 'r')
        byte_count += 5

        if byte_count >= 1200:
            s.sendall(str_buffer) # send a packet at a time. (1200ish bytes to be safe)
            str_buffer = "" # Empty out the buffer.
            byte_count = 0 # reset our byte count to 0
            s.recv(1500)

    s.sendall(str_buffer) # send a packet at a time. (1200ish bytes to be safe)
    str_buffer = "" # Empty out the buffer.
    byte_count = 0 # reset our byte count to 0
    s.recv(1500)

    print 'Made %d individual calls to the server' % runs


    s.close()
