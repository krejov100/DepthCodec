import unittest
from DepthFunctions import F0, F1, F2, F3, function_from_id


class TestDepthFunctions(unittest.TestCase):
	def test_function_from_id(self):
		self.assertIsInstance(function_from_id(0), F0)
		self.assertIsInstance(function_from_id(1), F1)
		self.assertIsInstance(function_from_id(2), F2)
		self.assertIsInstance(function_from_id(3), F3)


if __name__ == '__main__':
    unittest.main()