import socket
import select
import signal

signal.signal(signal.SIGPIPE, signal.SIG_IGN)

acceptor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
acceptor.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
acceptor.setblocking(True)
acceptor.bind(('', 8083))
acceptor.listen(5)

poller = select.epoll()

print("wait...")

def show_evt_name(evt: int) -> str:
    if evt & select.EPOLLIN:
        print("EPOLLIN", end=' ')
    if evt & select.EPOLLOUT:
        print("EPOLLOUT", end=' ')
    if evt & select.EPOLLRDHUP:
        print("EPOLLRDHUP", end=' ')
    if evt & select.EPOLLHUP:
        print("EPOLLHUP", end=' ')
    if evt & select.EPOLLERR:
        print("EPOLLERR", end=' ')
    print("")

clients = {}

half_close = False

poller.register(acceptor.fileno(), select.EPOLLIN)

while True:
    for fd, evt in poller.poll():
        print(f"[poll] {fd=}, {evt=}, ", end='')
        show_evt_name(evt)

        if evt & select.EPOLLHUP:
            print(f"Peer RST, close connection!")
            poller.unregister(fd)
            clients.pop(fd)
            client.close()

        if evt & select.EPOLLIN:
            if fd == acceptor.fileno():
                client, addr = acceptor.accept()
                print(f"accept new client: fd={fd}, addr={addr}")
                client.setblocking(True)
                poller.register(client.fileno(), select.EPOLLIN | select.EPOLLOUT)
                clients[client.fileno()] = client
            else:
                client = clients[fd]
                data = client.recv(1024)
                if data:
                    print(data.decode())
                else:
                    clients.pop(fd)
                    client.close()
                
        if evt & select.EPOLLOUT:
            client = clients[fd]
            data = b"HTTP/1.0 200 OK\r\nServer: xWebServer/0.1\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: 11\r\n\r\nhello world"
            n = client.send(data)
            print(f"Send: {n}")




