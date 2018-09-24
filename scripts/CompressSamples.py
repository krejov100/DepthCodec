import matplotlib.pyplot as plt
import pyrealsense2 as rs
import numpy as np
import glob
import open3d

from DepthImageCodec import DepthImageCodec


def loadSampleFromFile(ros_file_path):
	print('loading from: ' + example)
	config = rs.config()
	config.enable_device_from_file(ros_file_path)
	pipeline = rs.pipeline()
	pipeline.start(config)
	frames = pipeline.wait_for_frames()
	intrin = frames.get_profile().as_video_stream_profile().get_intrinsics()
	depth = frames.get_depth_frame()
	depth_data = depth.as_frame().get_data()
	depth_image = np.asanyarray(depth_data)
	return depth_image, intrin

files = glob.glob('C:/sample_data/*')
for example in files:
	depth, intrin = loadSampleFromFile(files[6])


	tc = DepthImageCodec()
	tc.compress(depth)
	uncompressed = tc.uncompress()

	open3d_intrin = open3d.PinholeCameraIntrinsic(intrin.width, intrin.height, intrin.fx, intrin.fy, intrin.ppx, intrin.ppy)
	open3d_depth = open3d.Image(depth)
	open3d_uncompressed	= open3d.Image(uncompressed)

	open3d_pc1 = open3d.create_point_cloud_from_depth_image(open3d_depth, open3d_intrin)
	open3d_pc2 = open3d.create_point_cloud_from_depth_image(open3d_uncompressed, open3d_intrin)
	open3d_pc1.paint_uniform_color([0.1, 0.9, 0.1])
	open3d_pc2.paint_uniform_color([0.1, 0.1, 0.7])
	open3d.draw_geometries([open3d_pc1, open3d_pc2])
	open3d.draw_geometries([open3d_pc2])

	plt.figure()
	plt.imshow(depth)
	plt.figure()
	plt.imshow(uncompressed)
	plt.pause(0.1)
	plt.waitforbuttonpress()

