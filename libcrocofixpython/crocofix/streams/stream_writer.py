import asyncio
from .._crocofix import _Writer

class StreamWriter(_Writer):

    def __init__(self, writer):
        super().__init__()
        self.writer = writer

    def write(self, message, options):
        data = message.encode(options)
        self.writer.write(bytes(data, 'utf-8'))
        # self.writer.drain()
    
    def close(self):
        self.writer.close()