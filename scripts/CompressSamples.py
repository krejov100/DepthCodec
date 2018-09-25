import matplotlib.pyplot as plt
import pyrealsense2 as rs
import numpy as np
import glob
import open3d
from bitstream import BitStream
from DepthImageCodec import DepthImageCodec

def post_process(image):
	spatial_filter = rs.spatial_filter()
	temporal_filter = rs.temporal_filter()
	hole_filling_filter = rs.hole_filling_filter()

	filter_magnitude = rs.option.filter_magnitude
	filter_smooth_alpha = rs.option.filter_smooth_alpha
	filter_smooth_delta = rs.option.filter_smooth_delta

	spatial_filter.set_option(filter_magnitude, 2)
	spatial_filter.set_option(filter_smooth_alpha, 0.5)
	spatial_filter.set_option(filter_smooth_delta, 20)
	temporal_filter.set_option(filter_smooth_alpha, 0.4)
	temporal_filter.set_option(filter_smooth_delta, 20)

	# Apply the filters
	filtered_frame = spatial_filter.process(image)
	filtered_frame = temporal_filter.process(filtered_frame)
	filtered_frame = hole_filling_filter.process(filtered_frame)

	return filtered_frame


def loadSampleFromFile(ros_file_path, max_distance):
	print('loading from: ' + example)
	config = rs.config()
	config.enable_device_from_file(ros_file_path)
	pipeline = rs.pipeline()
	profile = pipeline.start(config)
	frames = pipeline.wait_for_frames()
	intrin = frames.get_profile().as_video_stream_profile().get_intrinsics()
	depth = frames.get_depth_frame()
	depth = post_process(depth)
	depth_data = depth.as_frame().get_data()
	depth_image = np.asanyarray(depth_data)
	depth_scale = profile.get_device().first_depth_sensor().get_depth_scale()
	max_value = max_distance / depth_scale
	depth_image[depth_image > max_value] = max_value

	return depth_image, intrin

data_path = '/home/spc/Desktop/DepthCodec/sample_data/*'
files = glob.glob(data_path)
assert files.__len__(), "no test data in " + data_path
for example in files:
	depth, intrin = loadSampleFromFile(files[1], 20)
	plt.figure()
	plt.imshow(depth)
	plt.pause(0.1)
	plt.waitforbuttonpress()

	open3d_depth = open3d.Image(depth)
	open3d_intrin = open3d.PinholeCameraIntrinsic(intrin.width, intrin.height, intrin.fx, intrin.fy, intrin.ppx, intrin.ppy)
	open3d_pc1 = open3d.create_point_cloud_from_depth_image(open3d_depth, open3d_intrin)
	open3d.draw_geometries([open3d_pc1])

	tc = DepthImageCodec()
	tc.compress(depth)
	bits = BitStream()
	bits = tc.encode(bits)
	print(depth.size * 16 / len(bits))
	uncompressed = tc.uncompress()


	open3d_uncompressed	= open3d.Image(uncompressed)
	open3d_pc2 = open3d.create_point_cloud_from_depth_image(open3d_uncompressed, open3d_intrin)
	open3d_pc1.paint_uniform_color([0.1, 0.9, 0.1])
	open3d_pc2.paint_uniform_color([0.1, 0.1, 0.7])
	open3d.draw_geometries([open3d_pc1, open3d_pc2])
	open3d.draw_geometries([open3d_pc2])


	plt.figure()
	plt.imshow(uncompressed)
	plt.pause(0.1)
	plt.waitforbuttonpress()

