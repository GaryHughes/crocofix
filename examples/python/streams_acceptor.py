#!/usr/bin/env python3

# export PYTHONPATH=build-Debug/libcrocofixpython/crocofix

import asyncio

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


async def main():
    
    server = await asyncio.start_server(run_session, '127.0.0.1', 8089)

    addrs = ', '.join(str(sock.getsockname()) for sock in server.sockets)
    print(f'Serving on {addrs}')

    async with server:
        await server.serve_forever()


asyncio.run(main())