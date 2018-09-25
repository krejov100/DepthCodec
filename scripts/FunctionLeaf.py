from bitstream import BitStream
from zope.interface import implementer
from DepthFunctions import DepthFunction, function_from_id
from Tree import LeafData


def encode_function_id(id, stream):
    stream.write(id // 2, bool)
    stream.write(id % 2, bool)
    return stream


def decode_function_id(stream):
    return stream.read(bool) * 2 + stream.read(bool)



@implementer(LeafData)
class FunctionLeaf:
    def __init__(self):
        self._depth_function = None

    def set_depth_function(self, depth_function: DepthFunction):
        self._depth_function = depth_function

    def get_depth_function(self):
        return self._depth_function

    def draw(self, cell):
        if self._depth_function is None:
            return
        bits = BitStream()
        bits = self._depth_function.encode(bits)
        self._depth_function.decode(bits)
        self._depth_function.uncompress(cell)

    def encode(self, stream: BitStream):
        id = self._depth_function.get_function_index()
        encode_function_id(id, stream)
        self._depth_function.encode(stream)
        return stream

    def decode(self, stream: BitStream):
        f = decode_function_id(stream)
        self.set_depth_function(function_from_id(f))
        self._depth_function.decode(stream)
        return stream



