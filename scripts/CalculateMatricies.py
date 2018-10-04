from Gradient import *
import cv2


# TODO Move contents of file for gradient calculation
# http://www.epixea.com/research/publications/data/Morvan2006a.pdf
# section 4.2.1
def v(n):
	"""compute u value of the lhs matrix"""
	return ((n ** 2) * (n + 1)) / 2


def u(n):
	"""compute v value of lhs matrix"""
	return ((n ** 2) * (n + 1) ** 2) / 4


def t(n):
	"""compute t value of lhs matrix"""
	return ((n ** 2) * (n + 1) * (2 * n + 1)) / 6


def inverse_lh(n):
	"""build the inverse of the lhs matrix, in reality these should be pre-computed for all sizes of n"""
	return np.matrix([[t(n), u(n), v(n)], [u(n), t(n), v(n)], [v(n), v(n), n ** 2]]).I


def rh(im):
	"""compute the rhs matrix for closed form solving of gradients"""
	result = np.zeros(3)
	for y in range(0, im.shape[0]):
		for x in range(0, im.shape[1]):
			result[0] += x * im[y, x]
			result[1] += y * im[y, x]
			result[2] += im[y, x]
	return result


def masked_rh(im, mask):
	"""prototype function to test masking of the rhs of the closed form solution"""
	vals = np.zeros(3)
	for y in range(0, im.shape[0]):
		for x in range(0, im.shape[1]):
			if mask[y, x] != 0:
				vals[0] += x * im[y, x]
				vals[1] += y * im[y, x]
			vals[2] += im[y, x]
	return vals


def compute_gradient(im):
	""" compute the gradient using the closed form solution, slight modification from the paper
	I had to mean zero the im patch otherwise the closed form is not correct """
	n = im.shape[0]
	return rh(im - np.mean(im)) * inverse_lh(n), np.mean(im)


def masked_compute_gradient(im, mask):
	""" prototype function to try and compute the gradient using a mask, but so far, does not work """
	n = im.shape[0]
	print(n)
	return masked_rh(im - np.mean(im), mask) * inverse_lh(n), np.mean(im)


def render_gradient(n, bs):
	"""helper function to draw a gradient im based on bs and size n"""
	return make_gradient(n, bs[0][0, 2] + bs[0][0, 0] + bs[0][0, 1], bs[0][0, 0], bs[0][0, 1]) + bs[1]


# TODO switch to testing code
def main():
	n = 200

	noise = np.random.normal(0, 40, (n, n))
	im = cv2.imread('bgExampleDepth.tif')
	# gradient = make_gradient(n, 400, 0.2, 0.343) + noise
	gradient = im[750:950, 100:300, 0]

	print("rh" + str(rh(gradient)))
	bs = compute_gradient(gradient)
	print("bs" + str(bs))

	plt.figure()
	grad2 = render_gradient(n, bs)

	print(gradient.min())
	print(gradient.max())
	print(grad2.min())
	print(grad2.max())
	plt.imshow(np.vstack([gradient, grad2]))

	plt.pause(0)


if __name__ == "__main__":
	main()
