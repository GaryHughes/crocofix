import socket
from .._crocofix import _Writer

class SocketWriter(_Writer):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket

    def write(self, message, options):
        data = message.encode(options)
        self.socket.send(bytes(data, 'utf-8'))
       
    def close(self):
        self.socket.shutdown(socket.SHUT_WR)