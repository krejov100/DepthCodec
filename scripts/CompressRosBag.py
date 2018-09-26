import cv2

import matplotlib.pyplot as plt
import pyrealsense2 as rs
import numpy as np
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
	#filtered_frame = hole_filling_filter.process(filtered_frame)

	return filtered_frame

class RosBagGenerator:
	def __init__(self, ros_file_path, max_distance):
		self.max_distance = max_distance
		config = rs.config()
		config.enable_device_from_file(ros_file_path)
		self.pipeline = rs.pipeline()
		self.profile = self.pipeline.start(config)
		self.depths = []

		frame_number = 0
		while True:
			frames = self.pipeline.wait_for_frames()
			if frames[0].get_frame_number() < frame_number:
				break
			frame_number = frames[0].get_frame_number()
			if frames:
				self.intrin = frames.get_profile().as_video_stream_profile().get_intrinsics()
				depth = frames.get_depth_frame()
				self.depths.append(depth)

	def grab(self):
		for depth in (self.depths):
			depth = post_process(depth)
			depth_data = depth.as_frame().get_data()
			depth_image = np.asanyarray(depth_data)
			depth_scale = self.profile.get_device().first_depth_sensor().get_depth_scale()
			max_value = self.max_distance / depth_scale
			depth_image[depth_image > max_value] = max_value

			yield depth_image, self.intrin


data_path = 'C:/sample_data/20180923_130647.bag'
rosbag = RosBagGenerator(data_path, 3)
bits = BitStream()
frames = 0
tcs = []
for depth, intrin in rosbag.grab():
	tc = DepthImageCodec()
	tc.compress(depth)
	tcs.append(tc)
	bits = tc.encode(bits)
	frames += 1

print(depth.size * 16 / len(bits))
print(frames)

plt.waitforbuttonpress()
for i in tcs:
	uncompressed = i.uncompress()

	plt.imshow(uncompressed)
	plt.pause(0.0001)
