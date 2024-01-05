import unittest
from crocofix import *

class TestFieldCollection(unittest.TestCase):

    def test_set_operation_enum(self):
        SetOperation.__members__ == {
            'REPLACE_FIRST': SetOperation.REPLACE_FIRST,
            'REPLACE_FIRST_OR_APPEND': SetOperation.REPLACE_FIRST_OR_APPEND,
            'APPEND': SetOperation.APPEND
        }

    def test_remove_operation_enum(self):
        RemoveOperation.__members__ == {
            'REMOVE_FIRST': RemoveOperation.REMOVE_FIRST,
            'REMOVE_ALL': RemoveOperation.REMOVE_ALL
        }

    def test_default_state(self):
        self.assertEqual(len(FieldCollection()), 0)
     
    def test_overwrite_non_existent_field(self):
        fields = FieldCollection()
        self.assertFalse(fields.set(100, "ASX", SetOperation.REPLACE_FIRST))
        self.assertEqual(len(fields), 0)
   
    def test_add_non_existent_field(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.REPLACE_FIRST_OR_APPEND))
        self.assertEqual(len(fields), 1)
        field = next(iter(fields))
        self.assertEqual(field.tag, 100)
        self.assertEqual(field.value, "ASX")
        
    def test_add_duplicate_field(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.REPLACE_FIRST_OR_APPEND))
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertEqual(len(fields), 2)
        first, second = iter(fields)
        self.assertEqual(first.tag, 100)
        self.assertEqual(first.value, "ASX")
        self.assertEqual(second.tag, 100)
        self.assertEqual(second.value, "ASX")
        
    def test_remove_first_non_existent_field_from_empty_collection(self):
        fields = FieldCollection()
        self.assertFalse(fields.remove(100, RemoveOperation.REMOVE_FIRST))

    def test_remove_all_non_existent_field_from_empty_collection(self):
        fields = FieldCollection()
        self.assertFalse(fields.remove(100, RemoveOperation.REMOVE_ALL))

    def test_remove_first_existent_field_from_populated_collection(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertEqual(len(fields), 2)
        self.assertTrue(fields.remove(100, RemoveOperation.REMOVE_FIRST))
        self.assertEqual(len(fields), 1)
    
    def test_remove_all_existent_field_from_populated_collection(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertEqual(len(fields), 3)
        self.assertTrue(fields.remove(100, RemoveOperation.REMOVE_ALL))
        self.assertEqual(len(fields), 0)
       
    def test_get_non_existent_field_from_empty_collection(self):
        fields = FieldCollection()
        with self.assertRaises(IndexError) as context:
            fields.get(100)
        self.assertEqual(str(context.exception), "field collection does not contain a field with Tag 100") 
    

    def test_get_field_non_existent_field_from_non_empty_collection(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))        
        with self.assertRaises(IndexError) as context:
            fields.get(40)
        self.assertEqual(str(context.exception), "field collection does not contain a field with Tag 40") 

    def test_get_existent_field(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(100, "ASX", SetOperation.APPEND))
        self.assertEqual(fields.get(100).value, "ASX")

    def test_set_field_object(self):
        fields = FieldCollection()
        self.assertTrue(fields.set(Field(100, 'ASX'), SetOperation.APPEND))
        self.assertEqual(len(fields), 1)
        self.assertEqual(fields.get(100).value, "ASX")
        
if __name__ == "__main__":
    unittest.main()