import cv2

import matplotlib.pyplot as plt
import pyrealsense2 as rs
import numpy as np
from bitstream import BitStream
from DepthImageCodec import DepthImageCodec
from Common.RosBagDepthGenerator import RosBagGenerator


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
