import unittest
from crocofix import *

class TestField(unittest.TestCase):

    def test_timestamp_format_enum(self):
        TimestampFormat.__members__ == {
            'SECONDS': TimestampFormat.SECONDS,
            'MILLISECONDS': TimestampFormat.MILLISECONDS
        }
    
    def test_timestamp_format_seconds(self):
        self.assertEqual(len(timestamp_string(TimestampFormat.SECONDS)), 17)
 
    def test_timestamp_format_milliseconds(self):
        self.assertEqual(len(timestamp_string(TimestampFormat.MILLISECONDS)), 21)
 
    def test_init(self):
        self.assertEqual(Field(0, True).value, "Y")
        self.assertEqual(Field(0, False).value, "N")
        self.assertEqual(Field(0, "value").value, "value")
        self.assertEqual(Field(0, 1.2345).value, "1.234500")
        self.assertEqual(Field(0, -1.2345).value, "-1.234500")
        self.assertEqual(Field(0, 567800).value, "567800")
        self.assertEqual(Field(0, -567800).value, "-567800")
        
if __name__ == "__main__":
    unittest.main()