#!/usr/bin/env python3

# export PYTHONPATH=build-Debug/libcrocofixpython/crocofix

import asyncio
import traceback

from crocofix.session import Session, Behaviour
from crocofix.streams import *

async def main():

    reader, writer = await asyncio.open_connection('127.0.0.1', 8089)

    scheduler = Scheduler()

    reader = StreamReader(reader)
    writer = StreamWriter(writer)
    
    session = Session(reader, writer, scheduler)

    session.LogonBehaviour = Behaviour.INITIATOR
    session.BeginString = "FIXT.1.1"
    session.SenderCompID = "INITIATOR"
    session.TargetCompID = "ACCEPTOR"

    session.information = lambda x: print("INFO: {}".format(x))
    session.warning = lambda x: print("WARN: {}".format(x))
    session.error = lambda x: print("ERROR: {}".format(x))

    session.state_changed = lambda before, after: print("STATE CHANGED FROM {} TO {}".format(before, after))

    # If we don't set a handler here we get an exception - fix that in the C++ code.
    reader.message_read = lambda message: print("READER READ: {}".format(message))

    session.message_sent = lambda message: print("SENT: {}".format(message))
    session.message_received = lambda message: print("RECEIVED: {}".format(message))

    try:
        session.open()

        # Replace the sleep loop with this
        # scheduler.run()

        while True:
            await asyncio.sleep(5000)
  
    except Exception as ex:
        print(ex)
        print(traceback.format_exc())


asyncio.run(main())
