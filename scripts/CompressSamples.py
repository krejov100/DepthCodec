import matplotlib.pyplot as plt
import pyrealsense2 as rs
import numpy as np
import glob

from DepthImageCodec import DepthImageCodec


def loadSampleFromFile(rosFilePath):
	print('loading from: ' + example)
	config = rs.config()
	config.enable_device_from_file(rosFilePath)
	pipeline = rs.pipeline()
	pipeline.start(config)
	frames = pipeline.wait_for_frames()
	depth = frames.get_depth_frame()
	depth_data = depth.as_frame().get_data()
	depth_image = np.asanyarray(depth_data)
	return depth_image


files = glob.glob('C:/sample_data/*')
for example in files:
	depth = loadSampleFromFile(example)

	tc = DepthImageCodec()
	tc.compress(depth)
	uncompressed = tc.uncompress()
	plt.figure()
	plt.imshow(depth)
	plt.figure()
	plt.imshow(uncompressed)
	plt.pause(0)
