from typing import Optional

from sklearn import metrics
from zope.interface import Interface, implementer
from bitstream import BitStream
from CalculateMatricies import *
import numpy as np
from Errors import peak_signal_to_noise


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
        self.__max_val = np.mean(cell)

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
        stream.write(np.float16(self.__p0).tobytes())
        stream.write(np.float16(self.__p1).tobytes())
        stream.write(np.float16(self.__p2).tobytes())
        return stream

    def decode(self, stream: BitStream):
        self.__mean = stream.read(np.uint16, 1)
        self.__p0 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
        self.__p1 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
        self.__p2 = np.frombuffer(stream.read(np.int8, 2), dtype=np.float16)
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