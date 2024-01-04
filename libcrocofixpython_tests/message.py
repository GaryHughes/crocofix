import unittest
from crocofix import *

class TestMessage(unittest.TestCase):
    
    def test_decode(self):
        text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        message = Message()
        result = message.decode(text)
        self.assertTrue(result.complete)
        self.assertEqual(result.consumed, len(text))
        # REQUIRE(message.fields().size() == 18);

    

        
if __name__ == "__main__":
    unittest.main()