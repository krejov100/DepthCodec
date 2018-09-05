import numpy as np
import matplotlib.pyplot as plt
import itertools

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
	print(count)
	print(len(masks))
	return masks


#TODO actualy test
def testOuterIndexToXY():
	coords = getPerimeterCoord(4)
	count = 0
	for a in itertools.combinations(coords,4):
		count += 1
	print(count)

def main():

	fig, ax = plt.subplots()
	fig.show()
	
	for mask in genMaskfromLines(4):
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



