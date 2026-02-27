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


# Benchmark for the fido protocol.
#
# Sends batches of 'set' queries and measures throughput.
# Each batch is 240 queries x 5 bytes = 1200 bytes.
if __name__ == "__main__":
    import time
    import statistics

    QUERIES_PER_BATCH = 240
    QUERY_SIZE = 5  # struct size: 4 byte uint + 1 byte char
    RESPONSE_SIZE = QUERIES_PER_BATCH * QUERY_SIZE
    BATCHES = 1250
    WARMUP_BATCHES = 50
    NUM_RUNS = 5

    host = '0.0.0.0'
    port = 8030

    def recv_exact(sock, nbytes):
        """Read exactly nbytes from sock, looping until complete."""
        data = b""
        while len(data) < nbytes:
            chunk = sock.recv(nbytes - len(data))
            if not chunk:
                raise ConnectionError("connection closed mid-read")
            data += chunk
        return data

    def run_benchmark(host, port):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host, port))

        # Warmup — discard these timings
        for a in range(1, WARMUP_BATCHES + 1):
            buf = b""
            for b in range(1, QUERIES_PER_BATCH + 1):
                buf += struct.pack("Ic", a * b, b's')
            s.sendall(buf)
            recv_exact(s, RESPONSE_SIZE)

        # Timed run
        batch_times = []
        queries = 0

        t_start = time.perf_counter()
        for a in range(1, BATCHES + 1):
            buf = b""
            for b in range(1, QUERIES_PER_BATCH + 1):
                buf += struct.pack("Ic", a * b, b's')

            t_batch = time.perf_counter()
            s.sendall(buf)
            recv_exact(s, RESPONSE_SIZE)
            batch_times.append(time.perf_counter() - t_batch)

            queries += QUERIES_PER_BATCH

        elapsed = time.perf_counter() - t_start
        s.close()

        return queries, elapsed, batch_times

    run_results = []
    for run in range(1, NUM_RUNS + 1):
        queries, elapsed, batch_times = run_benchmark(host, port)
        qps = queries / elapsed
        run_results.append(qps)

        print("Run %d: %d queries in %.3fs (%.0f queries/sec)" %
              (run, queries, elapsed, qps))
        print("  batch latency — min: %.4fs  max: %.4fs  avg: %.4fs  median: %.4fs" %
              (min(batch_times), max(batch_times),
               statistics.mean(batch_times), statistics.median(batch_times)))

    print("\nSummary over %d runs:" % NUM_RUNS)
    print("  median throughput: %.0f queries/sec" % statistics.median(run_results))
    print("  min: %.0f  max: %.0f" % (min(run_results), max(run_results)))

