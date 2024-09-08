import socket
import threading

from .._crocofix import _Reader, _Message

class SocketReader(_Reader):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket
        self.message = _Message()
        self.closeEvent = threading.Event()

    def process_data(self, data):
        leftovers = bytearray()
        while len(data) > 0:
            result = self.message.decode(data)
            if result.complete:
                self.dispatch_message_read(self.message)
                self.message.reset()
                data = data[result.consumed:]
                continue
            if result.consumed >= len(data):
                return bytearray()
            leftovers = data[result.consumed:]
            break
        return leftovers

    def read_messages(self):
        leftovers = bytearray()
        while True:
            data = self.socket.recv(4096)
            if len(data) == 0:
                break
            leftovers = self.process_data(leftovers + data)
        self.closeEvent.set()

    def open(self):
        threading.Thread(target=self.read_messages).start()
        
    def close(self):
        self.socket.shutdown(socket.SHUT_RD)
        # self.closeEvent.wait()
