import numpy as np
import matplotlib.pyplot as plt
from gradiant import *
import cv2


def v(n):
	"""compute u value of matrix"""
	return ((n**2)*(n+1))/2


def u(n):
	"""compute v value of matrix"""
	return ((n**2)*(n+1)**2)/4


def t(n):
	return ((n**2)*(n+1)*(2*n+1))/6


def inverse_lh(n):
	return np.matrix([[t(n), u(n), v(n)], [u(n), t(n), v(n)], [v(n), v(n), n**2]]).I

def rh(im):
	vals = np.zeros(3)
	for y in range(0, im.shape[0]):
		for x in range(0, im.shape[1]):
			vals[0] += x * im[y, x]
			vals[1] += y * im[y, x]
			vals[2] += im[y, x]
	return vals

def masked_rh(im, mask):
	vals = np.zeros(3)
	for y in range(0, im.shape[0]):
		for x in range(0, im.shape[1]):
			if mask[y,x] != 0:
				vals[0] += x * im[y, x]
				vals[1] += y * im[y, x]
			vals[2] += im[y, x]
	return vals

def compute_gradiant(im):
	n = im.shape[0]
	return rh(im - np.mean(im)) * inverse_lh(n), np.mean(im)

def masked_compute_gradiant(im, mask):
	n = im.shape[0]
	print(n)
	return masked_rh(im - np.mean(im), mask) * inverse_lh(n), np.mean(im)

def render_gradiant(n, bs):
	return make_gradiant(n, bs[0][0, 2] + bs[0][0, 0] + bs[0][0, 1], bs[0][0, 0], bs[0][0, 1]) + bs[1]

def main():
	n = 200

	noise = np.random.normal(0, 40, (n, n))
	im = cv2.imread('bgExampleDepth.tif')
	grad = make_gradiant(n, 400, 0.2, 0.343) + noise
	grad = im[750:950,100:300,0]

	print("rh" + str(rh(grad)))
	bs = compute_gradiant(grad)
	print("bs" + str(bs))

	plt.figure()
	grad2 = render_gradiant(n, bs)

	print(grad.min())
	print(grad.max())
	print(grad2.min())
	print(grad2.max())
	plt.imshow(np.vstack([grad, grad2]))

	plt.pause(0)


if __name__ == "__main__":
	main()
