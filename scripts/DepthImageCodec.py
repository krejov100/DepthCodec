import cv2
import matplotlib.pyplot as plt
from QuadTreeCodec import QuadTreeCodec
from TiledCodec import TiledCodec, BitStream
import numpy as np


class CodecFactory:
    def __init__(self):
        self.quad_size = 64

    def create_codec(self):
        return QuadTreeCodec(quad_tree_size=self.quad_size, min_cell_snr=70, debug=False)

    def get_codec_shape(self):
        return [self.quad_size, self.quad_size]


class DepthImageCodec(TiledCodec):
    def __init__(self):
        TiledCodec.__init__(self, CodecFactory())

    def compress(self, image: np.ndarray):
        super().compress(image)

    def uncompress(self) -> np.ndarray:
        return super().uncompress()

    def encode(self, stream: BitStream):
        super().encode(stream)

    def decode(self, stream: BitStream):
        super().decode(stream)


def main():
    im = cv2.imread('bgExampleDepth.tif')[:, :, 1]
    tc = DepthImageCodec()
    tc.compress(im)
    uncompressed = tc.uncompress()
    plt.figure()
    plt.imshow(im)
    plt.figure()
    plt.imshow(uncompressed)
    plt.pause(0.001)
    plt.waitforbuttonpress()
