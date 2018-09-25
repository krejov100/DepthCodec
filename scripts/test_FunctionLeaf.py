import unittest
from bitstream import BitStream

from DepthFunctions import F1
from FunctionLeaf import FunctionLeaf, decode_function_id, encode_function_id


class MockF1(F1):
	def __init__(self):
		self._max_val = 2234

class TestFunctionLeaf(unittest.TestCase):
	def test_encode_function_id(self):
		bits = encode_function_id(0, BitStream())
		self.assertEqual(bits, BitStream([False, False]))
		bits = encode_function_id(1, BitStream())
		self.assertEqual(bits, BitStream([False, True]))
		bits = encode_function_id(2, BitStream())
		self.assertEqual(bits, BitStream([True, False]))
		bits = encode_function_id(3, BitStream())
		self.assertEqual(bits, BitStream([True, True]))

	def test_encode_function_id(self):
		f = FunctionLeaf()
		self.assertEqual(0, decode_function_id(BitStream([False, False])))
		self.assertEqual(1, decode_function_id(BitStream([False, True])))
		self.assertEqual(2, decode_function_id(BitStream([True, False])))
		self.assertEqual(3, decode_function_id(BitStream([True, True])))

	def test_encode_decode(self):
		fl = FunctionLeaf()
		fl2 = FunctionLeaf()
		fl.set_depth_function(MockF1())
		bits = BitStream()
		bits = fl.encode(bits)
		fl2.decode(bits)
		self.assertAlmostEquals(fl2.get_depth_function().get_max_val(), fl.get_depth_function().get_max_val())


if __name__ == '__main__':
    unittest.main()