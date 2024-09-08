import unittest
from crocofix.streams.stream_reader import StreamReader

# This is a mock asyncio.StreamReader, the count argument to read() is ignored and
# it returns successive chunks of data from an array passed to the constructor. This
# lets the caller specify the read() behaviour simply for test code.
class TestReader:

    def __init__(self, chunks):
        self.chunks = chunks
        self.current_chunk = 0

    async def read(self, count):
        if self.current_chunk >= len(self.chunks):
            return []
        chunk = self.chunks[self.current_chunk]
        self.current_chunk += 1
        return bytes(chunk, 'utf8')


class TestStreamReader(unittest.IsolatedAsyncioTestCase):

    async def test_complete_message_in_single_read(self):
        chunks = [ "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001" ]
        messages = []
        reader = StreamReader(TestReader(chunks))
        reader.message_read = lambda message : messages.append(message)
        await reader.read_messages()
        self.assertEqual(len(messages), 1)

    async def test_complete_message_in_two_pieces_aligned_on_a_field_boundary(self): 
        chunks = [ 
            "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u0001",
            "54=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        ]
        messages = []
        reader = StreamReader(TestReader(chunks))
        reader.message_read = lambda message : messages.append(message)
        await reader.read_messages()
        self.assertEqual(len(messages), 1)

    async def test_decode_a_complete_message_in_two_pieces_not_aligned_on_a_field_boundary(self): 
        chunks = [ 
            "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=B",
            "HP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        ]
        messages = []
        reader = StreamReader(TestReader(chunks))
        reader.message_read = lambda message : messages.append(message)
        await reader.read_messages()
        self.assertEqual(len(messages), 1)

    async def test_multiple_complete_messages_in_single_read(self):
        chunks = [ "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u00018=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001" ]
        messages = []
        reader = StreamReader(TestReader(chunks))
        reader.message_read = lambda message : messages.append(message)
        await reader.read_messages()
        self.assertEqual(len(messages), 2)


if __name__ == "__main__":
    unittest.main()
