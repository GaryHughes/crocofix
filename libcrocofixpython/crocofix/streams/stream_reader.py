import asyncio
from .._crocofix import _Reader, _Message

class StreamReader(_Reader):

    def __init__(self, reader):
        super().__init__()
        self.reader = reader
        self.message = _Message()
        self.task = None

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


    async def read_messages(self):
        leftovers = bytearray()
        while True:
            data = await self.reader.read(4096)
            if len(data) == 0:
                break
            leftovers = self.process_data(leftovers + data)

    def open(self):
        loop = asyncio.get_running_loop()
        self.task = loop.create_task(self.read_messages())
   
    def close(self):
        # how do we do this? do we need to?
        pass
       