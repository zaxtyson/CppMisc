import socket
import signal
import time
import os

signal.signal(signal.SIGPIPE, signal.SIG_IGN)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
client.setblocking(True)
client.connect(('127.0.0.1', 8081))

client.send(b"GET /hello HTTP/1.0\r\n\r\n")

client.shutdown(socket.SHUT_WR)
print(client.recv(1024))
# client.close()
time.sleep(3)
# while True:
#     print(client.recv(1024))

"""
Watch               Client          EpollWait
IN/OUT/RDHUP        close           IN/OUT/RDHUP
IN/OUT/RDHUP        shutdown        IN/OUT/RDHUP
IN                  close           IN(r=0)
IN                  shutdown        IN(r=0)
IN                  exit/abort      IN(r=0)
"""
