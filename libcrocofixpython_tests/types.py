import unittest
from crocofix import *

class TestTypes(unittest.IsolatedAsyncioTestCase):

    def test_behaviour_enum(self):
        Behaviour.__members__ == {
            'INITIATOR' : Behaviour.INITIATOR,
            'ACCEPTOR'  : Behaviour.ACCEPTOR
        }

    def test_session_state_enum(self):
        SessionState.__members__ == {
            'DISCONNECTED'  : SessionState.DISCONNECTED,
            'CONNECTED'     : SessionState.CONNECTED,
            'LOGGING_ON'    : SessionState.LOGGING_ON,
            'RESENDING'     : SessionState.RESENDING,
            'LOGGED_ON'     : SessionState.LOGGED_ON,
            'RESETTING'     : SessionState.RESETTING
        }

if __name__ == "__main__":
    unittest.main()

