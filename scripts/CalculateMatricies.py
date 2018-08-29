import numpy as np
import matplotlib.pyplot as plt

def v(n):
	"""compute u value of matrix"""
	return (n**2)*(n+1)/2


def u(n):
	"""compute v value of matrix"""
	return (n**2)*((n+1)**2)/4

def t(n):
	return (n**2)*(n+1)*(2*n+1)/6

def inverseLh(n):
	return np.matrix([[t(n), u(n), v(n)],[u(n), t(n), v(n)],[v(n), v(n), n**2]]).I

def rh(im):
	vals = np.zeros(3)
	for y in xrange(0, im.shape[0]):
		for x in xrange(0, im.shape[1]):
			vals[0] += (x * im[y,x])
			vals[1] += (y * im[y,x])
			vals[2] += im[y,x]
	return vals;

def makeGradiant(n, b0 ,b1, b2):
	rslt = np.zeros((n,n))
	for y in xrange(0, rslt.shape[0]):
		for x in xrange(0, rslt.shape[1]):
			rslt[y,x] = b0 + (b1*x) + (b2*y)
	return rslt

def main():
	n = 100
	noise = np.random.normal(20, 10,(n,n))
	grad = makeGradiant(n,0,100,100) + noise
	plt.imshow(grad)


	print("rh" + str(rh(grad)))
	bs =  rh(grad)* inverseLh(n)
	print("bs"+ str(bs))

	plt.figure()
	grad = makeGradiant(n,bs[0,2],bs[0,0],bs[0,1])
	plt.imshow(grad)

	plt.pause(0)
if __name__ == "__main__":
    main()
