import glob
import pyrealsense2 as rs
from errors import *
import numpy as np
import matplotlib.pyplot as plt
from sklearn.feature_extraction import image
from CalculateMatricies import *
import multiprocessing


files = glob.glob('/home/spc/Desktop/DepthCodec/sampleData/*')
print(files)


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


def showCellAndDecoding(cell, decoded, min, max):
	plt.imshow(np.hstack((cell, decoded)), vmin=min, vmax=max)
	plt.pause(0.001)



def f0error(cell):
	decoded = np.zeros(cell.shape)
	cell_mse = mse(cell, decoded)
	if(cell_mse == 0):
		return(0,float('Inf'))
	return cell_mse, peak_signal_to_noise(cell, decoded)


def f1error(cell):
	decoded = np.zeros(cell.shape)
	max = np.amax(cell)
	decoded.fill(max)
	cell_mse = mse(cell, decoded)
	if(cell_mse == 0):
		return(0,float('Inf'))
	#if(psnr(cell, decoded) < 1000000 and psnr(cell, decoded) > 1):
	#		showCellAndDecoding(cell, decoded)
	return cell_mse, peak_signal_to_noise(cell, decoded)


def f2error(cell, b2):
	bs = compute_gradiant(cell)
	n = cell.shape[0]
	max = np.amax(cell)
	min = np.amin(cell)
	decoded = render_gradiant(n, bs)
	cell_mse = mse(cell, decoded)
	if (cell_mse == 0):
		return 0, float('Inf'), bs
	#if (psnr(cell, decoded) < 1000000 and psnr(cell, decoded) > 60):
	#	showCellAndDecoding(cell, decoded, min, max)
	return cell_mse, peak_signal_to_noise(cell, decoded), bs


def f3error(cell):
	pass


def f4error(cell):
	pass

n = 8
cell_sizes = [ 2**j for j in range(1,n+1)]
print(cell_sizes)
f0_count = {}
f1_count = {}
f2_count = {}
for cell_size in cell_sizes:
	f0_count[cell_size] = 0
	f1_count[cell_size] = 0
	f2_count[cell_size] = 0

for example in files:
	depth = loadSampleFromFile(example)
	for cell_size in cell_sizes:
		cells = image.extract_patches_2d(depth, (cell_size, cell_size), 100 ,42)
		for cell in cells:
			f0 = f0error(cell)[1]
			f1 = f1error(cell)[1]
			f2 = f2error(cell)[1]
			bs = f2error(cell)[2]
			print(bs)
			if f0 > 80 or f1 > 80 or f2 > 80:
				if f0 >= f1 and f0 >= f2:
					f0_count[cell_size] += 1
				if f1 >= f0 and f1 >= f2:
					f1_count[cell_size] += 1
				if f2 >= f0 and f2 >= f1:
					f2_count[cell_size] += 1

	print(f0_count)
	print(f1_count)
	print(f2_count)
