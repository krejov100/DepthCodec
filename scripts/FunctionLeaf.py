from bitstream import BitStream
from zope.interface import implementer
from DepthFunctions import DepthFunction
from Tree import LeafData


@implementer(LeafData)
class FunctionLeaf:
    def __init__(self):
        self._depth_function = None

    def set_depth_function(self, depth_function: DepthFunction):
        self._depth_function = depth_function

    def draw(self, cell):
        if self._depth_function is None:
            return
        bits = BitStream()
        bits = self._depth_function.encode(bits)
        self._depth_function.decode(bits)
        self._depth_function.uncompress(cell)
