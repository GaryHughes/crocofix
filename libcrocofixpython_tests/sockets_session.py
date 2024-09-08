import unittest
import socket
from crocofix.session import Session, Behaviour
from crocofix.sockets.scheduler import Scheduler
from crocofix.sockets.socket_reader import SocketReader
from crocofix.sockets.socket_writer import SocketWriter

class TestSession(unittest.TestCase):

    def create_acceptor(self):
        pass

    def create_initiator(self):
        pass

    def test_session(self):
   
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(('localhost', 8089))
        client.setblocking(False)

        scheduler = Scheduler()

        reader = SocketReader(client)
        writer = SocketWriter(client)
        
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

        # while True:
        #     time.sleep(50000)

     

if __name__ == "__main__":
    unittest.main()
