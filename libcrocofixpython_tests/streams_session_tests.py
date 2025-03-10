import unittest
import asyncio
from crocofix.session import Session, Behaviour
from crocofix.streams.scheduler import Scheduler
from crocofix.streams.socket_reader import SocketReader
from crocofix.streams.socket_writer import SocketWriter

class TestSession(unittest.IsolatedAsyncioTestCase):

    async def test_session(self):
   
        reader, writer = await asyncio.open_connection('127.0.0.1', 8089)

        scheduler = Scheduler()

        reader = SocketReader(reader)
        writer = SocketWriter(writer)
        
        session = Session(reader, writer, scheduler)

        session.LogonBehaviour = Behaviour.INITIATOR
        session.BeginString = "FIXT.1.1"
        session.SenderCompID = "INITIATOR"
        session.TargetCompID = "ACCEPTOR"
 
        session.information = lambda x: print("INFO: {}".format(x))
        session.warning = lambda x: print("WARN: {}".format(x))
        session.error = lambda x: print("ERROR: {}".format(x))

        session.state_changed = lambda before, after: print("STATE CHANGED FROM {} TO {}".format(before, after))

        session.message_sent = lambda message: print("SENT: {}".format(message))
        session.message_received = lambda message: print("RECEIVED: {}".format(message))

        session.open()

        # Replace the sleep loop with this
        # scheduler.run()

        while True:
            await asyncio.sleep(50000)

        # loop = asyncio.get_running_loop()
        # await loop.run_until_complete()  
     

if __name__ == "__main__":
    unittest.main()
