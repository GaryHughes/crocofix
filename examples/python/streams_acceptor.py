#!/usr/bin/env python3

# export PYTHONPATH=build-Debug/libcrocofixpython/crocofix

import asyncio
import logging

from crocofix.session import Session, Behaviour
from crocofix.streams import *

async def run_session(reader, writer):

    scheduler = Scheduler()

    reader = StreamReader(reader)
    writer = StreamWriter(writer)
    
    session = Session(reader, writer, scheduler)

    session.LogonBehaviour = Behaviour.ACCEPTOR
    session.BeginString = "FIXT.1.1"
    session.SenderCompID = "ACCEPTOR"
    session.TargetCompID = "INITIATOR"

    session.information = lambda message: logging.info(message)
    session.warning = lambda message: logging.warning(message)
    session.error = lambda message: logging.error(message)

    session.state_changed = lambda before, after: logging.info("STATE %s -> %s", before, after)
    session.message_sent = lambda message: logging.info("OUT %s", message)
    session.message_received = lambda message: logging.info("IN %s", message)

    session.open()

    # TODO - handle session disconnect and exit from this function

    # Replace the sleep loop with this
    # scheduler.run()

    while True:
        await asyncio.sleep(50000)


async def main():
    
    logging.basicConfig(format='%(asctime)s %(levelname)s: %(message)s', level=logging.INFO)

    server = await asyncio.start_server(run_session, '127.0.0.1', 8089)

    addrs = ', '.join(str(sock.getsockname()) for sock in server.sockets)
    logging.info(f'Listening on {addrs}')

    async with server:
        await server.serve_forever()


asyncio.run(main())