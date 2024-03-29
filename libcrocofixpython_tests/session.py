import unittest
import socket
import asyncio
import traceback
from crocofix import *

class SocketReader(Reader):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket
        self.message = Message()

    def process_data(self, data):
        while len(data) > 0:        
            result = self.message.decode(data)
            if result.complete:
                print("IN " + str(self.message))
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
           print(traceback.format_exc()) 
           
    def open(self):
        loop = asyncio.get_event_loop()
        loop.create_task(self.read_messages())

    def close(self):
        self.socket.shutdown(socket.SHUT_RD)
    


class SocketWriter(Writer):

    def __init__(self, socket):
        super().__init__()
        self.socket = socket

    def write(self, message, options):
        print("OUT " + str(message))
        data = message.encode()
        self.socket.send(bytes(data, 'utf-8'))
       
    def close(self):
        self.socket.shutdown(socket.SHUT_WR)
       

class AsyncIoScheduler(Scheduler):

    def __init__(self):
        super().__init__()
        self.tasks = set()
  
    def run(self):
        pass
     
    def schedule(self, task):
        # This is used by test code in the C++ implementation so we don't need it here.
        raise NotImplementedError()
 
    def schedule_relative_callback(self, when, callback):
        loop = asyncio.get_running_loop()
        loop.call_later(when.total_seconds(), callback)
        # TODO
        return 0
    
    async def repeating_callback(self, interval, callback):
        while True:
            await asyncio.sleep(interval.total_seconds())
            callback() 
  
    def schedule_repeating_callback(self, interval, callback):
        task = asyncio.create_task(self.repeating_callback(interval, callback))
        task.add_done_callback(self.tasks.discard)
        # TODO
        return 0
 
    def cancel_callback(self, token):
        # TODO - we need this for session shutdown
        raise NotImplementedError()


class TestSession(unittest.IsolatedAsyncioTestCase):

    def test_behaviour_enum(self):
        Behaviour.__members__ == {
            'INITIATOR': Behaviour.INITIATOR,
            'ACCEPTOR': Behaviour.ACCEPTOR
        }

    async def test_session(self):
    
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(('localhost', 8089))
        client.setblocking(False)

        scheduler = AsyncIoScheduler()

        reader = SocketReader(client)
        writer = SocketWriter(client)
        
        session = Session(reader, writer, scheduler)

        session.LogonBehaviour = Behaviour.INITIATOR
        session.BeginString = "FIXT.1.1"
        session.SenderCompID = "INITIATOR"
        session.TargetCompID = "ACCEPTOR"
    
        # session.message_sent
        # session.message_received

        session.open()
        # scheduler.run()

        # loop = asyncio.get_event_loop()
        # loop.run_forever()
        while True:
            await asyncio.sleep(50000)
       


if __name__ == "__main__":
    unittest.main()

