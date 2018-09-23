import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt

from zope.interface import implementer
import pyrealsense2 as rs
from typing import Optional
from errors import *
from QuadTreeNode import QuadTreeNode
from Tree import Tree, LeafData, LeafDataFactory
from Rect import Rect
from bitstream import BitStream
from bitarray import bitarray
from PIL import Image
from DepthFunctions import *

def get_peak_signal_to_noise(image: np.ndarray, f: DepthFunction):
    uncompressed = np.zeros(image.shape)
    f.compress(image)
    bits = BitStream()
    bits = f.encode(bits)
    f.decode(bits)
    f.uncompress(uncompressed)
    if mse(image, uncompressed) == 0:
        return float('Inf')
    sig = peak_signal_to_noise(image, uncompressed)
    return sig


# noinspection
def get_best_function(image: np.ndarray, min_psnr: float) -> Optional[DepthFunction]:
    f0_codec: DepthFunction = F0()
    f0_psnr = get_peak_signal_to_noise(image, f0_codec)
    if f0_psnr > min_psnr:
        return f0_codec

    if np.amin(image) == 0:
        return None

    f1_codec: DepthFunction = F1()
    f1_psnr = get_peak_signal_to_noise(image, f1_codec)
    if f1_psnr > min_psnr:
        return f1_codec

    if image.shape[0] > 4:
        f2_codec: DepthFunction = F2()
        f2_psnr = get_peak_signal_to_noise(image, f2_codec)
        if f2_psnr > min_psnr:
            return f2_codec

        # f3_codec: DepthFunction = F3()
        # f3_psnr = get_peak_signal_to_noise(image, f3_codec)
        # if f3_psnr > min_psnr:
        #    return f3_codec
    return None


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


@implementer(LeafDataFactory)
class CompressedLeafFactory:
    def __init__(self, min_patch_snr:float):
        self.min_patch_snr = min_patch_snr
        self.__data_stream = BitStream()
        self.split_counts = [{}, {}, {}, {}]
        n = 10
        po2s = [2 ** j for j in range(0, n + 1)]
        for i in range(4):
            for po2 in po2s:
                self.split_counts[i][po2] = 0

    # noinspection PyMethodMayBeStatic
    def should_split(self, node: QuadTreeNode):
        # reached bottom
        if node.roi.width == 1 or node.roi.height == 1:
            return False
        sub_image = node.get_sub_image()

        best_function = get_best_function(sub_image, self.min_patch_snr)
        return best_function is None

    # noinspection PyMethodMayBeStatic
    def create_leaf_data(self, node: QuadTreeNode) -> FunctionLeaf:
        # this currently means the function sets are calculated twice,
        # only has compression time hit, and should be optimised out
        fl = FunctionLeaf()
        fl.set_depth_function(get_best_function(node.get_sub_image(), self.min_patch_snr))
        node.set_leaf_data(fl)

        self.split_counts[fl._depth_function.get_function_index()][node.roi.width] += 1
        return fl

class QuadTreeCodec:
    def __init__(self, quad_tree_size=64):
        self.quad_tree_size = quad_tree_size
        self.tree = None
    def get_codec_shape(self):
        return [self.quad_tree_size, self.quad_tree_size]

    def compress(self, image: np.ndarray):
        root = QuadTreeNode(image, Rect(0, 0, self.quad_tree_size, self.quad_tree_size))
        self.tree = Tree(root)
        leaf_factory = CompressedLeafFactory(70.0)
        self.tree.top_down(leaf_factory)

    # noinspection PyMethodMayBeStatic
    def uncompress(self, cell: np.ndarray):
        self.tree.draw(cell, False)

    def encode(self, stream: BitStream):
        return stream

    def decode(self, stream: BitStream):
        return stream


class TiledCodec:
    def __init__(self, codec_factory):
        self.codecs = []
        self.original_size = []
        self.codec_factory = codec_factory

    def compress(self, image: np.ndarray):
        self.original_size = np.asarray(image.shape)
        codec_shape = self.codec_factory.get_codec_shape()
        # resize the image to a compatible size, this for prototype purpose
        new_size = (self.original_size // codec_shape) * codec_shape
        # having to use pil and openCV bug #9096
        image = np.asarray(Image.fromarray(image).resize((new_size[1], new_size[0]), Image.NEAREST))
        for y in range(0, new_size[0], codec_shape[0]):
            for x in range(0, new_size[1], codec_shape[1]):
                codec = self.codec_factory.create_codec()
                sub_image = Rect(x, y, codec_shape[0], codec_shape[1]).sub_image(image)
                codec.compress(sub_image)
                self.codecs.append(codec)

    def uncompress(self) -> np.ndarray:
        codec_shape = self.codecs[0].get_codec_shape()
        new_size = (self.original_size // codec_shape) * codec_shape
        image = np.zeros((new_size[0], new_size[1]))
        index = 0
        for y in range(0, new_size[0], codec_shape[0]):
            for x in range(0, new_size[1], codec_shape[1]):
                sub_image = Rect(x, y, codec_shape[0], codec_shape[1]).sub_image(image)
                self.codecs[index].uncompress(sub_image)
                index += 1
        return image

    def encode(self, stream: BitStream):
        for codec in self.codecs:
            codec.encode(stream)

    def decode(self, stream: BitStream):
        for codec in self.codecs:
            codec.decode(stream)


class CompressedDepthImage:
    def compress(self, image: np.ndarray):
        pass

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return


def main():
    class CodecFactory:
        def __init__(self):
            self.quad_size = 32

        def create_codec(self):
            return QuadTreeCodec(self.quad_size)

        def get_codec_shape(self):
            return [self.quad_size, self.quad_size]

    im = cv2.imread('bgExampleDepth.tif')[:, :, 1]
    tc = TiledCodec(CodecFactory())
    tc.compress(im)
    uncompressed = tc.uncompress()
    plt.figure()
    plt.imshow(im)
    plt.figure()
    plt.imshow(uncompressed)
    plt.pause(0)


def main3():
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

    @implementer(LeafDataFactory)
    class CompressedLeafFactory:
        def __init__(self):
            self.__data_stream = BitStream()

            self.split_counts = [{}, {}, {}, {}]
            n = 10
            po2s = [2 ** j for j in range(0, n + 1)]
            for i in range(4):
                for po2 in po2s:
                    self.split_counts[i][po2] = 0


        # noinspection PyMethodMayBeStatic
        def should_split(self, node: QuadTreeNode):
            # reached bottom
            if node.roi.width == 1 or node.roi.height == 1:
                return False
            sub_image = node.get_sub_image()

            best_function = get_best_function(sub_image, 84)
            return best_function is None

        # noinspection PyMethodMayBeStatic
        def create_leaf_data(self, node: QuadTreeNode) -> FunctionLeaf:
            # this currently means the function sets are calculated twice,
            # only has compression time hit, and should be optimised out
            fl = FunctionLeaf()
            fl.set_depth_function(get_best_function(node.get_sub_image(), 84))
            node.set_leaf_data(fl)

            self.split_counts[fl._depth_function.get_function_index()][node.roi.width] += 1
            return fl

    # TODO get a 16bit test image
    im = cv2.imread('bgExampleDepth.tif')[:,:,1]
    tc = TiledCodec()
    tc.compress(im)
    # resize image to nearest power of two, compress
    # im = cv2.resize(im, (1024, 1024), 0, 0, cv2.INTER_NEAREST).astype(np.uint16).copy()
    # having to use pil and openCV bug #9096
    im = np.asarray(Image.fromarray(im).resize((1024, 1024), Image.NEAREST))


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

    @implementer(LeafDataFactory)
    class CompressedLeafFactory:
        def __init__(self):
            self.__data_stream = BitStream()

            self.split_counts = [{}, {}, {}, {}]
            n = 10
            po2s = [2 ** j for j in range(0, n + 1)]
            for i in range(4):
                for po2 in po2s:
                    self.split_counts[i][po2] = 0

    root = QuadTreeNode(im, Rect(0, 0, 1024, 1024))
    tree = Tree(root)
    Factory = CompressedLeafFactory()
    tree.top_down(Factory)

    # using asanyarray improves print
    for i in range(4):
        print([x for x in Factory.split_counts[i].values()])

    decompressed = np.zeros(im.shape)
    tree.draw(decompressed, False)

    print(peak_signal_to_noise(im, decompressed))

    plt.imshow(np.abs(im - decompressed), vmax=5, vmin=0)
    plt.figure()

    plt.imshow(im)
    plt.figure()

    plt.imshow(decompressed)
    plt.pause(0)



def main2():
    # Configure depth and color streams
    pipeline = rs.pipeline()
    config = rs.config()
    config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
    config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)

    # Start streaming
    pipeline.start(config)
    intrin = pipeline.get_active_profile().get_streams()[0].as_video_stream_profile().get_intrinsics()
    try:
        while True:

            # Wait for a coherent pair of frames: depth and color
            frames = pipeline.wait_for_frames()
            depth_frame = frames.get_depth_frame()
            color_frame = frames.get_color_frame()
            if not depth_frame or not color_frame:
                continue

            # Convert images to numpy arrays
            depth_image = np.asanyarray(depth_frame.get_data())
            color_image = np.asanyarray(color_frame.get_data())

            # open3d_depth_img = open3d.Image((depth_image * 255).astype(np.uint8))
            # open3d.draw_geometries([open3d_depth_img])

            # pcd = PointCloud()
            # pcd.points = Vector3dVector(xyz)
            # write_point_cloud("../../TestData/sync.ply", pcd)

            # Apply colormap on depth image (image must be converted to 8-bit per pixel first)
            depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)

            # Stack both images horizontally
            images = np.hstack((color_image, depth_colormap))

            # Show images
            cv2.namedWindow('RealSense', cv2.WINDOW_AUTOSIZE)
            cv2.imshow('RealSense', images)
            cv2.waitKey(1)

    finally:
        # Stop streaming
        pipeline.stop()


if __name__ == '__main__':
    main()
