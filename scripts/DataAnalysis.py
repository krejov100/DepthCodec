import glob
from Errors import *
from sklearn.feature_extraction import image
from CalculateMatricies import *

# TODO Comment
def showCellAndDecoding(cell, decoded, min, max):
	plt.imshow(np.hstack((cell, decoded)), vmin=min, vmax=max)
	plt.pause(0.001)

# TODO Comment
def f0error(cell):
	decoded = np.zeros(cell.shape)
	cell_mse = mean_squared_error(cell, decoded)
	if(cell_mse == 0):
		return(0,float('Inf'))
	return cell_mse, peak_signal_to_noise(cell, decoded)

# TODO Comment
def f1error(cell):
	decoded = np.zeros(cell.shape)
	max = np.amax(cell)
	decoded.fill(max)
	cell_mse = mean_squared_error(cell, decoded)
	if(cell_mse == 0):
		return(0,float('Inf'))
	#if(psnr(cell, decoded) < 1000000 and psnr(cell, decoded) > 1):
	#		showCellAndDecoding(cell, decoded)
	return cell_mse, peak_signal_to_noise(cell, decoded)

# TODO Comment
def f2error(cell, b2):
	bs = compute_gradiant(cell)
	n = cell.shape[0]
	max = np.amax(cell)
	min = np.amin(cell)
	decoded = render_gradient(n, bs)
	cell_mse = mean_squared_error(cell, decoded)
	if (cell_mse == 0):
		return 0, float('Inf'), bs
	#if (psnr(cell, decoded) < 1000000 and psnr(cell, decoded) > 60):
	#	showCellAndDecoding(cell, decoded, min, max)
	return cell_mse, peak_signal_to_noise(cell, decoded), bs

# TODO Comment
def f3error(cell):
	pass

# TODO Comment
def f4error(cell):
	pass

# TODO Move to scripts folder
files = glob.glob('./sample_data*')
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
