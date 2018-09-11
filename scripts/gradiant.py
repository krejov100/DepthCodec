import numpy as np
import matplotlib.pyplot as plt
import itertools
import cv2
import copy
from CalculateMatricies import *
from Tree import *

def make_gradiant(n, b0 ,b1, b2):
	rslt = np.zeros((n,n))
	for y in range(0, rslt.shape[0]):
		for x in range(0, rslt.shape[1]):
			rslt[y,x] = b0 + (b1*x) + (b2*y)
	return rslt

#def makeSplitGradiant(n, )

def ABmask(n, p1, p2):
	rslt = np.zeros((n,n))
	for y in range(0, rslt.shape[0]):
		for x in range(0, rslt.shape[1]):
			rslt[y,x] = leftOrRight(n, p1, p2)
		
#  is p3 on the left of a line connecting p1 and p2
def isPleftOfLine(p1, p2, p3):
	m = np.ones((3,3))
	m[0,1:3] = p1
	m[1,1:3] = p2
	m[2,1:3] = p3
	return np.sign(np.linalg.det(m)) > 0
	
def testIsPleftOfLine():
	assert(leftOrRight([2,-1], [2,6], [4,0]) < 0)
	assert(leftOrRight([2,-1], [2,6], [4,2]) < 0)
	assert(leftOrRight([2,-1], [2,6], [4,4]) < 0)
	assert(leftOrRight([2,-1], [2,6], [0,0]) > 0)
	assert(leftOrRight([2,-1], [2,6], [0,4]) > 0)
	leftOrRight([3,4], [6,7], [8,8])

def getPerimeterCoord(n):
	perimeterCord = np.empty((0,2), int)
	index = 0
	#top
	for x in range(-1, n):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[x,-1]]), axis=0)
	#right
	for y in range(-1, n):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[n,y]]), axis=0)
	#bottom
	for x in range(n, -1, -1):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[x,n]]), axis=0)
	#left
	for y in range(n, -1, -1):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[-1,y]]), axis=0)

	return perimeterCord

def top(n):
	perimeterCord = np.empty((0,2), int)
	index = 0
	#top
	for x in range(-1, n):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[x,-1]]), axis=0)
	return perimeterCord

def right(n):
	perimeterCord = np.empty((0,2), int)
	index = 0
	#top
	for y in range(-1, n):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[n,y]]), axis=0)
	return perimeterCord

def bottom(n):
	perimeterCord = np.empty((0,2), int)
	index = 0
	#top
	for x in range(n, -1, -1):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[x,n]]), axis=0)
	return perimeterCord

def left(n):
	perimeterCord = np.empty((0,2), int)
	index = 0
	#top
	for y in range(n, -1, -1):
		index+=1
		perimeterCord = np.append(perimeterCord, np.array([[-1,y]]), axis=0)
	return perimeterCord


# returns an image that is masked using a line P Q, 0 if right of the line, 1 if left
def maskImageUsingLine(n, p1, p2):
	rslt = np.zeros((n,n))
	for y in range(0, n):
		for x in range(0, n):
			rslt[y,x] = isPleftOfLine(p1, p2, [x,y])
	return rslt

#should look like the example in paper
# TODO replace with an assertable test
def testMaskImageUsingLine():
	plt.imshow(maskImageUsingLine(6, [3, -1], [1,8]))
	plt.pause(0)

def appendMask(array, mask):
	for alreadyin in array:
		if (alreadyin == mask).all():
			return array
		#line creates a region that is tool small
		if(mask.sum() < 4 or (1-mask).sum() < 4):
			return array
	array.append(mask)
	return array	
	

# compute the LUT for masks that corrispond to a line deviding the image
# This could be reduced to just two cases, top-bottom and top to left, which are then rotated to provide the other 4 cases
def genMaskfromLines(n):
	masks = []

	topCoords = top(n)
	rightCoords = right(n)
	bottomCoords = bottom(n)
	leftCoords = left(n)

	count = 0
	#from top-right
	for a in topCoords:
		for b in rightCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from right-bottom
	for a in rightCoords:
		for b in bottomCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from bottom-left
	for a in bottomCoords:
		for b in leftCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from left-top
	for a in leftCoords:
		for b in topCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from top-bottom
	for a in topCoords:
		for b in bottomCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from right-left
	for a in rightCoords:
		for b in leftCoords:
			count += 1
			masks = appendMask(masks, maskImageUsingLine(n, a, b))	
	return masks


#TODO actualy test
def testOuterIndexToXY():
	coords = getPerimeterCoord(8)
	count = 0
	for a in itertools.combinations(coords,8):
		count += 1


masks = genMaskfromLines(8)

def masked_mean_center(im, mask):
	im_with_nan = copy.deepcopy(im)
	im_with_nan[mask == 0] = np.nan
	mean = np.nanmean(im_with_nan)
	return (mask*im) + ((1-mask)*mean)

def find_gradiant_split(im):
	# 7x7 because there are less then 255 line masks
	split_gradient_small = cv2.resize(im, (8, 8), 0, 0, cv2.INTER_NEAREST)

	fig, ax = plt.subplots()
	fig.show()

	errors = []
	index = 0
	best_score = 0
	best_result = []
	for mask in masks[1:]:
		index += 1
		m = mask
		# compute gradient for mask
		gradient = masked_compute_gradiant(masked_mean_center(split_gradient_small, m), m)
		rendered_gradient = render_gradiant(8, gradient)

		# compute gradient for inverse mask
		m = 1-mask
		gradient = masked_compute_gradiant(masked_mean_center(split_gradient_small, m), m)
		rendered_gradient2 = render_gradiant(8, gradient)

		rslt = (rendered_gradient2 * m) + (rendered_gradient * (1-m))

		psnr_val = psnr(split_gradient_small, rslt)
		errors.append(psnr_val )

		if psnr_val > best_score:
			best_result = np.vstack((split_gradient_small, m, rslt, split_gradient_small-rslt))
			best_score = psnr_val
	best_result = cv2.resize(best_result, (50,50), 0, 0)


	plt.imshow(best_result)

	fig.canvas.draw()
	plt.waitforbuttonpress()
	ax.cla()
	plt.close(fig)


def main():

	fig, ax = plt.subplots()
	fig.show()

	for i in range(0,241,4):
		print(i)
		test_50_mask = maskImageUsingLine(50, (0,50), (50,10))
		#					    one gradiant 											second gradiant
		#test_gradient = (masks[i] * make_gradiant(8,10, 0.3,-0.5))  +  ((1- masks[i]) * make_gradiant(8, 40, 0.1, 0.4))
		test_gradient = (test_50_mask * make_gradiant(50, 10, 0.3, -0.5)) + ((1 - test_50_mask) * make_gradiant(50, 40, 0.1, 0.4))

		find_gradiant_split(test_gradient)

	print(len(genMaskfromLines(8)))
	for mask in genMaskfromLines(8):
		ax.imshow(mask)
		fig.canvas.draw()
		ax.cla()
		#plt.pause(0.01)
	#genMaskfromLines(16)
	#testMaskImageUsingLine()
	#maskImageUsingLine()
	#testOuterIndexToXY()

if __name__ == "__main__":
    main()



