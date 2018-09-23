from zope.interface import implementer
from bitstream import BitStream
from CalculateMatricies import *
import numpy as np


class DepthFunction(Interface):
    def compress(self, cell: np.ndarray):
        pass

    def uncompress(self, cell: np.ndarray):
        pass

    # noinspection PyMethodMayBeStatic
    def encode(self, stream: BitStream) -> BitStream:
        return stream

    # noinspection PyMethodMayBeStatic
    def decode(self, stream: BitStream) -> BitStream:
        return stream

    def get_function_index(self):
        pass


@implementer(DepthFunction)
class F0:
    def compress(self, cell: np.ndarray):
        pass

    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        cell.fill(0)

    # noinspection PyMethodMayBeStatic
    def encode(self, stream: BitStream):
        return stream

    # noinspection PyMethodMayBeStatic
    def decode(self, stream: BitStream):
        return stream

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 0


@implementer(DepthFunction)
class F1:
    def __init__(self):
        self.__max_val = 0

    def compress(self, cell: np.ndarray):
        self.__max_val = np.amax(cell)

    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        cell.fill(self.__max_val)

    def encode(self, stream: BitStream):
        stream.write(self.__max_val, np.uint16)
        return stream

    def decode(self, stream: BitStream):
        self.__max_val = stream.read(np.uint16, 1)[0]
        return stream

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 1


@implementer(DepthFunction)
class F2:
    def __init__(self):
        self.__mean = 0
        self.__p0 = 0
        self.__p1 = 0
        self.__p2 = 0

    def compress(self, cell: np.ndarray):
        (p, self.__mean) = compute_gradiant(cell)
        self.__p0 = p[0, 0]
        self.__p1 = p[0, 1]
        self.__p2 = p[0, 2]

    def uncompress(self, cell: np.ndarray):
        cell[:, :] = render_gradiant(cell.shape[0], (np.asanyarray([[self.__p0, self.__p1, self.__p2]]), self.__mean))

    def encode(self, stream: BitStream):
        stream.write(self.__mean, np.uint16)
        stream.write(self.__p0, np.float)
        stream.write(self.__p1, np.float)
        stream.write(self.__p2, np.float)
        return stream

    def decode(self, stream: BitStream):
        self.__mean = stream.read(np.uint16, 1)
        self.__p0 = stream.read(np.float, 1)
        self.__p1 = stream.read(np.float, 1)
        self.__p2 = stream.read(np.float, 1)
        return stream

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 2


@implementer(DepthFunction)
class F3:
    def compress(self, cell: np.ndarray):
        pass

    def uncompress(self, cell: np.ndarray):
        pass

    def encode(self, stream: BitStream):
        return

    def decode(self, stream: BitStream):
        return

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 3
