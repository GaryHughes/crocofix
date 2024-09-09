#!/usr/bin/env python3

# export PYTHONPATH=build-Debug/libcrocofixpython/crocofix

import socket
import logging

from crocofix.session import Session, Behaviour
from crocofix.sockets import *

if __name__ == "__main__":

    logging.basicConfig(format='%(asctime)s %(levelname)s: %(message)s', level=logging.INFO)

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(('localhost', 8089))
  
    scheduler = Scheduler()

    reader = SocketReader(client)
    writer = SocketWriter(client)

    session = Session(reader, writer, scheduler)

    session.LogonBehaviour = Behaviour.INITIATOR
    session.BeginString = "FIXT.1.1"
    session.SenderCompID = "INITIATOR"
    session.TargetCompID = "ACCEPTOR"

    session.information = lambda message: logging.info(message)
    session.warning = lambda message: logging.warning(message)
    session.error = lambda message: logging.error(message)

    session.state_changed = lambda before, after: logging.info("STATE %s -> %s", before, after)
    session.message_sent = lambda message: logging.info("OUT %s", message)
    session.message_received = lambda message: logging.info("IN %s", message)

    # TODO - handle session disconnect and scheduler exit

    session.open()
    scheduler.run()
