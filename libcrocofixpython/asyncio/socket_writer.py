import socket
from crocofix import Writer

class SocketWriter(Writer):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket

    def write(self, message, options):
        data = message.encode(options)
        self.socket.send(bytes(data, 'utf-8'))
       
    def close(self):
        self.socket.shutdown(socket.SHUT_WR)