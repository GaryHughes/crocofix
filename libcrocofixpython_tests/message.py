import unittest
from crocofix import *

class TestMessage(unittest.TestCase):

    def test_encode_options(self):
        self.assertEqual(EncodeOptions.NONE, 0)
        self.assertEqual(EncodeOptions.SET_CHECKSUM, 1)
        self.assertEqual(EncodeOptions.SET_BODY_LENGTH, 2)
        self.assertEqual(EncodeOptions.SET_BEGIN_STRING, 4)
        self.assertEqual(EncodeOptions.SET_MSG_SEQ_NUM, 8)
        self.assertEqual(EncodeOptions.STANDARD, EncodeOptions.SET_CHECKSUM | EncodeOptions.SET_BODY_LENGTH | EncodeOptions.SET_BEGIN_STRING | EncodeOptions.SET_MSG_SEQ_NUM)

    def test_decode(self):
        text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        message = Message()
        result = message.decode(text)
        self.assertTrue(result.complete)
        self.assertEqual(result.consumed, len(text))
        self.assertEqual(len(message.fields), 18)

    def test_decode_a_complete_message_in_two_pieces_aligned_on_a_field_boundary(self): 
        one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u0001"
        two = "54=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message()
        result = message.decode(one)
        self.assertFalse(result.complete)
        self.assertEqual(result.consumed, len(one))
        result = message.decode(two)
        self.assertTrue(result.complete)
        self.assertEqual(result.consumed, len(two))

    def test_decode_a_complete_message_in_two_pieces_not_aligned_on_a_field_boundary(self): 
        one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=B"
        two = "55=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message()
        result = message.decode(one)
        self.assertFalse(result.complete)
        self.assertEqual(result.consumed, len(one) - len('55=B'))
        result = message.decode(two)
        self.assertTrue(result.complete)
        self.assertEqual(result.consumed, len(two))

    def test_invalid_tag_throws(self):
        message = Message()
        self.assertRaises(IndexError, message.decode, 'A=FIX.4.4')


    def test_MsgType_lookup_throws_for_a_message_with_no_MsgType(self):
        text = "8=FIX.4.4\u00019=149\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message() 
        message.decode(text)
        self.assertRaises(RuntimeError, lambda m: m.MsgType, message)

    def test_MsgType_lookup(self):
        text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message()
        message.decode(text)
        self.assertEqual(message.MsgType, 'D')

    def test_is_admin_is_false_for_a_non_admin_message(self):
        text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message()    
        message.decode(text)
        self.assertFalse(message.is_admin)

    def test_is_admin_is_true_for_an_admin_message(self): 
        text = "8=FIX.4.4\u00019=149\u000135=A\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        message = Message()
        message.decode(text)
        self.assertTrue(message.is_admin)

    def test_encode_a_message(self): 
        expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001"
        message = Message()
        message.decode(expected)
        actual = message.encode()
        self.assertEqual(actual, expected)

    def test_format_checksum_greater_than_3_digits_throws(self): 
        self.assertRaises(RuntimeError, Message.format_checksum, 9999)
  
    def test_format_checksum_pads_values_with_less_than_3_digits(self): 
        self.assertEqual(Message.format_checksum(999), "999")
        self.assertEqual(Message.format_checksum(99), "099")
        self.assertEqual(Message.format_checksum(9), "009")
        self.assertEqual(Message.format_checksum(0), "000")
        self.assertEqual(Message.format_checksum(90), "090")
        self.assertEqual(Message.format_checksum(900), "900")
         
if __name__ == "__main__":
    unittest.main()