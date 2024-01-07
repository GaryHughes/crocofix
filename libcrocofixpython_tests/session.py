import unittest
import socket
from crocofix import *

class SocketReader(Reader):

    def read_async(self, callback):
        print('SocketReader.read_async') 

    def read_async2(self, callback):
        print('SocketReader.read_async') 

    def do_stuff(self, value):
        print('SocketReader.do_stuff {}'.format(value))

    def do_more_stuff(self, value):
        print('SocketReader.do_more_stuff')

    def do_int_stuff(self, value):
        print('SocketReader.do_int_stuff')

    def close(self):
        print('SocketReader.close')

class SocketWriter(Writer):

    def write(self, message, options):
        print('SocketWriter.write')

    def close(self):
        print('SocketWriter.close')


class TestSession(unittest.TestCase):

    def test_session(self):

        # client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # client.connect(('localhost', 8089))

        reader = SocketReader()
        writer = SocketWriter()
        session = Session(reader, writer)

        # session.open()




if __name__ == "__main__":
    unittest.main()

