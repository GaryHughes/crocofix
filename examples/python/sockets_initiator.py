#!/usr/bin/env python3

# export PYTHONPATH=build-Debug/libcrocofixpython/crocofix

import socket
import traceback

from crocofix.session import Session, Behaviour, SessionState
from crocofix.sockets import *

if __name__ == "__main__":

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(('localhost', 8089))
    # client.setblocking(False)

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

    def on_state_changed(before, after):
        print("STATE CHANGED FROM {} TO {}".format(before, after))
        # if after == SessionState.LOGGED_ON:
        #     print("SESSION HAS LOGGED ON")
        #     scheduler.stop()

    session.state_changed = on_state_changed

    # If we don't set a handler here we get an exception - fix that in the C++ code.
    reader.message_read = lambda message: print("READER READ: {}".format(message))

    session.message_sent = lambda message: print("SENT: {}".format(message))
    session.message_received = lambda message: print("RECEIVED: {}".format(message))
  
    try:
        session.open()
        scheduler.run()
    except Exception as ex:
        print(ex)
        print(traceback.format_exc())