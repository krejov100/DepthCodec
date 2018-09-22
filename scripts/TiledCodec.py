import pyrealsense2 as rs
import numpy as np
import cv2
from CalculateMatricies import *
from errors import *
from QuadTreeNode import QuadTreeNode
from Tree import Tree
from Rect import Rect
from bitstream import BitStream
from bitarray import bitarray


def get_psnr(image: np.ndarray, f):
    deccompressed = image.copy()
    f.compress(image)
    bits = BitStream()
    bits = f.encode(bits)
    f.decode(bits)
    deccompressed = f.decompress(deccompressed)

    if mse(image, deccompressed) == 0:
        return float('Inf')
    f_psnr = psnr(image, deccompressed)
    return f_psnr


class F0:
    def compress(self, cell:np.ndarray):
        pass

    def decompress(self, cell: np.ndarray):
        cell = np.zeros(cell.shape)
        return cell
    def encode(self, stream: BitStream):
        return stream

    def decode(self, stream: BitStream):
        return stream


class F1:
    def __init__(self):
        self.__max_val = 0

    def compress(self, cell: np.ndarray):
        self.__max_val = np.amax(cell)


    def decompress(self, cell: np.ndarray):
        cell.fill(self.__max_val)
        return cell

    def encode(self, stream: BitStream):
        stream.write(self.__max_val, np.uint16)
        return stream

    def decode(self, stream: BitStream):
        self.__max_val = stream.read(np.uint16, 1)[0]
        return stream


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

    def decompress(self, cell: np.ndarray):
        cell = render_gradiant(cell.shape[0], (np.asanyarray([[self.__p0, self.__p1, self.__p2]]), self.__mean))
        return cell

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

class F3:
    def compress(self, cell: np.ndarray):
        pass

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return


def get_best_function(image: np.ndarray, min_psnr: float):
    best_psnr = 0
    # order of evaluating best F is from biggest bit budget to smallest
    if image.shape[0] > 1:
        f2_codec = F2()
        f2_psnr = get_psnr(image, f2_codec)
        print( "f2_psnr",  f2_psnr)
        if f2_psnr <= best_psnr:
            best_psnr =  f2_psnr
            best_codec = f2_codec
        #f3_codec = F3()
        #f3_psnr = getPSNR(image, f3_codec)
        #if f3_psnr <= best_psnr:
        #    best_psnr = f3_psnr
        #    best_codec = f3_codec

    f1_codec = F1()
    f1_psnr = get_psnr(image, f1_codec)
    print("f1_psnr", f1_psnr)
    if f1_psnr <= best_psnr:
        best_psnr = f1_psnr
        best_codec = f1_codec
    f0_codec = F0()
    f0_psnr = get_psnr(image, f0_codec)
    print("f0_psnr", f0_psnr)
    if f0_psnr <= best_psnr:
        best_psnr = f0_psnr
        best_codec = f0_codec
    if best_psnr < min_psnr:
        return None
    else:
        return best_codec



class QuadTreeCodec:
    def compress(self, image:np.ndarray):
        # hear we use the tree class with write out of it split or not

        node = Node(QuadROILeaf())
        self.__tree = Tree(Rect(0,0,image.shape[0],image.shape[1]), )
        self.__tree.top_down(image)


    def decompress(self, bits: bitarray, cell: np.ndarray):
        # hear we use stream the deciding if we split
        pass

    def get_compressed_bit_stream(self):
        return


class TiledCodec:
    def compress(self, image: np.ndarray):
        pass

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return


class CompressedDepthImage:
    def compress(self, image:np.ndarray):
        pass

    def decompress(self, bits: bitarray, cell: np.ndarray):
        pass

    def get_compressed_bit_stream(self):
        return



def main():

    class FunctionLeaf:
        def __init__(self, im):
            self.f = None
        def draw(self, im):
            if self.f is None:
                return
            bits = BitStream()
            bits = self.f.encode(bits)
            bits = self.f.decode(bits)
            self.f.decompress(im)

    class CompressedLeafFactory:
        def __init__(self):
            self.__datasteam = BitStream()

        def should_split(self, node: QuadTreeNode):
            # reached bottom
            if node.roi.width == 1:
                return False
            best_fucntion = get_best_function(node.get_sub_image(), 70)
            return best_fucntion is None

        def create_leaf_data(self, node: QuadTreeNode):
            # this currently means the function sets are calculated twice,
            # only has compression time hit, and should be optimised out
            self.f = get_best_function(node.get_sub_image(), 70)
            return FunctionLeaf(node.roi.sub_im(node.image))


    # TODO get a 16bit test image
    im = cv2.imread('bgExampleDepth.tif')
    original_size = im.shape
    im = cv2.resize(im[:, :, 1], (64, 64), 0, 0, cv2.INTER_NEAREST).astype(np.uint16).copy()


    root = QuadTreeNode(im, Rect(0, 0, 64, 64))
    tree = Tree(root)
    tree.top_down(CompressedLeafFactory())

    decompressed = im.copy()
    tree.root.draw(decompressed, True)

    #plt.imshow(im - decompressed)
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
