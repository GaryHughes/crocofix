import asyncio
import socket
from crocofix import Reader, Message

class SocketReader(Reader):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket
        self.message = Message()

    def process_data(self, data):
        while len(data) > 0:        
            result = self.message.decode(data)
            if result.complete:
                self.dispatch_message_read(self.message)
                self.message.reset()
            if len(data) <= result.consumed:
                break
            data = data[result.consumed:]

    async def read_messages(self):
        try:
            loop = asyncio.get_running_loop()
            while True:
                data = await loop.sock_recv(self.socket, 8192)
                if len(data) == 0:
                    self.close()
                    return
                self.process_data(data)
        except Exception as ex:
           print(ex)
           
    def open(self):
        loop = asyncio.get_running_loop()
        loop.create_task(self.read_messages())
   
    def close(self):
        self.socket.shutdown(socket.SHUT_RD)
        # TODO - wait for task, probably via an event the task signals