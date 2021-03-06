import unittest
from typing import Optional
from sklearn import metrics
from zope.interface import implementer
from bitstream import BitStream
from CalculateMatricies import *
import numpy as np
from Errors import peak_signal_to_noise


# TODO Comment
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


# TODO Comment
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


# TODO Comment
@implementer(DepthFunction)
class F1:
    def __init__(self):
        self._max_val = 0

    def compress(self, cell: np.ndarray):
        self._max_val = np.mean(cell)

    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        cell.fill(self._max_val)

    def encode(self, stream: BitStream):
        stream.write(self._max_val, np.uint16)
        return stream

    def decode(self, stream: BitStream):
        self._max_val = stream.read(np.uint16, 1)[0]
        return stream

    def get_max_val(self):
        return self._max_val

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 1


# TODO Comment
@implementer(DepthFunction)
class F2(unittest.TestCase):
    def __init__(self):
        self._mean = 0
        self._p0 = 0
        self._p1 = 0
        self._p2 = 0

    def compress(self, cell: np.ndarray):
        (p, self.__mean) = compute_gradient(cell)
        self._p0 = p[0, 0]
        self._p1 = p[0, 1]
        self._p2 = p[0, 2]

    def uncompress(self, cell: np.ndarray):
        cell[:, :] = render_gradient(cell.shape[0], (np.asanyarray([[self._p0, self._p1, self._p2]]), self._mean))

    def encode(self, stream: BitStream):
        stream.write(self.__mean, np.uint16)
        stream.write(np.float16(self._p0).tobytes())
        stream.write(np.float16(self._p1).tobytes())
        stream.write(np.float16(self._p2).tobytes())
        return stream

    def decode(self, stream: BitStream):
        self._mean = stream.read(np.uint16, 1)
        self._p0 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
        self._p1 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
        self._p2 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
        return stream

    # noinspection PyMethodMayBeStatic
    def get_function_index(self):
        return 2


# implimentation of the function 3 from platlett based compression.
# the function is based on a line divide the quad into two sub regions.
# Each sub region is represented using a single depth value.
@implementer(DepthFunction)
class F3:
    def __init__(self):
        self.lineMasks=

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


# TODO Comment
def function_from_id(function_id):
    if function_id == 0:
        return F0()
    if function_id == 1:
        return F1()
    if function_id == 2:
        return F2()
    if function_id == 3:
        return F3()


# TODO Comment
def get_peak_signal_to_noise(image: np.ndarray, f: DepthFunction):
    uncompressed = np.zeros(image.shape)
    f.compress(image)
    bits = BitStream()
    bits = f.encode(bits)
    f.decode(bits)
    f.uncompress(uncompressed)
    if metrics.mean_squared_error(image, uncompressed) == 0:
        return float('Inf')
    sig = peak_signal_to_noise(image, uncompressed)
    return sig


# TODO Comment
# noinspection
def get_best_function(image: np.ndarray, min_psnr: float) -> Optional[DepthFunction]:

    #f0_codec: DepthFunction = F0()
    #f0_psnr = get_peak_signal_to_noise(image, f0_codec)
    #if f0_psnr > min_psnr:

    if np.amax(image) == 0:
        return F0()

    if np.amin(image) == 0:
       return None

    min = np.amin(image)
    min /= 0xfff
    min_psnr -= (min) * 20

    f1_codec = F1()
    f1_psnr = get_peak_signal_to_noise(image, f1_codec)
    if f1_psnr > min_psnr:
        return f1_codec

    if image.shape[0] > 4:
        f2_codec = F2()
        f2_psnr = get_peak_signal_to_noise(image, f2_codec)
        if f2_psnr > min_psnr:
            return f2_codec

        # f3_codec: DepthFunction = F3()
        # f3_psnr = get_peak_signal_to_noise(image, f3_codec)
        # if f3_psnr > min_psnr:
        #    return f3_codec
    return None
