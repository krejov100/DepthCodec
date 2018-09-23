from zope.interface import Interface, implementer
import pyrealsense2 as rs
from typing import Optional
from CalculateMatricies import *
from errors import *
from QuadTreeNode import QuadTreeNode
from Tree import Tree, LeafData, LeafDataFactory
from Rect import Rect
from bitstream import BitStream
from bitarray import bitarray
from PIL import Image


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

    def get_function_index(self):
        return 2

@implementer(DepthFunction)
class F3:
    def compress(self, cell: np.ndarray):
        pass

    def uncompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return

    def get_function_index(self):
        return 3

# noinspection
def get_best_function(image: np.ndarray, min_psnr: float) -> Optional[DepthFunction]:
    f0_codec: DepthFunction = F0()
    f0_psnr = get_peak_signal_to_noise(image, f0_codec)
    if f0_psnr > min_psnr:
        return f0_codec

    f1_codec: DepthFunction = F1()
    f1_psnr = get_peak_signal_to_noise(image, f1_codec)
    if f1_psnr > min_psnr:
        return f1_codec

    if image.shape[0] > 2:# and np.amin(image) != 0:
        f2_codec: DepthFunction = F2()
        f2_psnr = get_peak_signal_to_noise(image, f2_codec)
        if f2_psnr > min_psnr-5:
            return f2_codec

        # f3_codec: DepthFunction = F3()
        # f3_psnr = get_peak_signal_to_noise(image, f3_codec)
        # if f3_psnr > min_psnr:
        #    return f3_codec

    return None



class QuadTreeCodec:

    def compress(self, image: np.ndarray):
        # hear we use the tree class with write out of it split or not
        # node = Node(QuadROILeaf())
        # self.__tree = Tree(Rect(0,0,image.shape[0],image.shape[1]), )
        # self.__tree.top_down(image)
        pass

    # noinspection PyMethodMayBeStatic
    def decompress(self, bits: bitarray, cell: np.ndarray):
        # hear we use stream the deciding if we split
        pass

    def get_compressed_bit_stream(self):
        return


class TiledCodec:
    def compress(self, image: np.ndarray):
        

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return


class CompressedDepthImage:
    def compress(self, image: np.ndarray):
        pass

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return


def main():
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
    # resize image to nearest power of two, compress
    #im = cv2.resize(im, (1024, 1024), 0, 0, cv2.INTER_NEAREST).astype(np.uint16).copy()
    # having to use pil and openCV bug #9096


    im = np.asarray(Image.fromarray(im).resize((1024, 1024), Image.NEAREST))

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
