import numpy as np
import matplotlib.pyplot as plt
import itertools

def makeGradiant(n, b0 ,b1, b2):
	rslt = np.zeros((n,n))
	for y in xrange(0, rslt.shape[0]):
		for x in xrange(0, rslt.shape[1]):
			rslt[y,x] = b0 + (b1*x) + (b2*y)
	return rslt

#def makeSplitGradiant(n, )

def ABmask(n, p1, p2):
	rslt = np.zeros((n,n))
	for y in xrange(0, rslt.shape[0]):
		for x in xrange(0, rslt.shape[1]):
			rslt[y,x] = leftOrRight(n, p1, p2)		
		

def leftOrRight(n, p1, p2):
	pass
	


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

def testOuterIndexToXY():
	coords = getPerimeterCoord(2)
	for a in itertools.combinations(coords,2):
		print(a)

def main():
	testOuterIndexToXY()

if __name__ == "__main__":
    main()


