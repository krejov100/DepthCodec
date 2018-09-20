import pyrealsense2 as rs
import numpy as np
import cv2

from Tree import Tree
import bitarray


def getPSNR(image:np.ndarray, f):
    deccompressed = image.clone()
    f.compress(image)
    f.decompress(deccompressed)
    f_psnr = PSNR(image, deccompressed)
    return f_psnr

class F0:
    def compress(self, image:np.ndarray):

    def decompress(self, image: np.ndarray):

    def get_compressed_bit_stream(self):
        return


class F1:
    def compress(self, image:np.ndarray):

    def decompress(self, image: np.ndarray):

    def get_compressed_bit_stream(self):
        return


class F2:
    def compress(self, image:np.ndarray):

    def decompress(self, image: np.ndarray):

    def get_compressed_bit_stream(self):
        return


class F3:
    def compress(self, image:np.ndarray):

    def decompress(self, image: np.ndarray):

    def get_compressed_bit_stream(self):
        return


def get_best_function(image:np.ndarray, min_psnr:float):
    best_psnr = 0
    # order of evaluating best F is from biggest bit budget to smallest
    if image.shape[0] > 1:
        f2_codec = F2()
        f2_psnr = getPSNR(image, f2_codec)
        if f2_psnr <= best_psnr:
            best_psnr =  f2_psnr
            best_codec = f2_codec
        f3_codec = F3()
        f3_psnr = getPSNR(image, f3_codec)
        if f3_psnr <= best_psnr:
            best_psnr = f3_psnr
            best_codec = f3_codec
    else:
        f1_codec = F1()
        f1_psnr = getPSNR(image, f1_codec)
        if f1_psnr <= best_psnr:
            best_psnr = f1_psnr
            best_codec = f1_codec
        f0_codec = F0()
        f0_psnr = getPSNR(image, f0_codec)
        if f0_psnr <= best_psnr:
            best_psnr = f0_psnr
            best_codec = f0_codec
    if best_psnr < min_psnr:
        return None
    else:
        return best_codec



class QuadTreeCodec:
    def compress(self, image:np.ndarray):

    def decompress(self, image: np.ndarray):

    def get_compressed_bit_stream(self):
        return


class TiledCodec:
    def compress(self, image: np.ndarray):

    def decompress(self, image:np.ndarray):

    def get_compressed_bit_stream(self):
        return


class CompressedDepthImage:
    def compress(self, image:np.ndarray):

    def decompress(self, image:np.ndarray):

    def get_compressed_bit_stream(self):
        return




def main():
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
