from gradiant import *
import numpy as np
import matplotlib.pyplot as plt
import math
from itertools import *

perimeter_hops = 4

def top_coordinates(n):
	step = math.ceil(n/perimeter_hops)
	perimeter_cord = np.empty((0, 2), int)
	for x in range(0, n, step):
		perimeter_cord = np.append(perimeter_cord, np.array([[x, 0]]), axis=0)
	return perimeter_cord


def right_coordinates(n):
	step = math.ceil(n/perimeter_hops)
	perimeter_cord = np.empty((0, 2), int)
	for y in range(0, n, step):
		perimeter_cord = np.append(perimeter_cord, np.array([[n, y]]), axis=0)
	return perimeter_cord


def bottom_coordinates(n):
	step = math.ceil(n/perimeter_hops)
	perimeter_cord = np.empty((0, 2), int)
	for x in range(n, 0, -step):
		perimeter_cord = np.append(perimeter_cord, np.array([[x, n]]), axis=0)
	return perimeter_cord


def left_coordinates(n):
	step = math.ceil(n/perimeter_hops)
	perimeter_cord = np.empty((0, 2), int)
	for y in range(n, 0, -step):
		perimeter_cord = np.append(perimeter_cord, np.array([[0, y]]), axis=0)
	return perimeter_cord


def compute_perimeterlines(n):
	masks = []
	top = top_coordinates(n)
	right = right_coordinates(n)
	bottom = bottom_coordinates(n)
	left = left_coordinates(n)

	#from top-right
	for a in top:
		for b in right:
			print(a,b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from right-bottom
	for a in right:
		for b in bottom:
			print(a, b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from bottom-left
	for a in bottom:
		for b in left:
			print(a, b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from left-top
	for a in left:
		for b in top:
			print(a, b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from top-bottom
	for a in top:
		for b in bottom:
			print(a, b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	#from right-left
	for a in right:
		for b in left:
			print(a, b)
			masks = appendMask(masks, maskImageUsingLine(n, a, b))
	return masks


n = 16
step = math.ceil(n/perimeter_hops)
top_ids = list(product([0], range(0, n-1, step)))
right_ids = list(product(range(0, n-1, step), [n-1]))
bottom_ids = list(product([n-1], range(n-1, 0, -step)))
left_ids = list(product(range(n-1, 0, -step), [0]))

a = list(permutations(chain(top_ids, right_ids, bottom_ids, left_ids), 2))
masks = []
for pair in a:
	if pair[0][0] == pair[1][0] or pair[0][1] == pair[1][1]:
		continue
	print(pair)
	masks.append(maskImageUsingLine(n, pair[1], pair[0]))

fig, ax = plt.subplots()
fig.show()
for m  in masks:
	ax.imshow(m)
	fig.canvas.draw()
	#plt.waitforbuttonpress()
	ax.cla()

print(top_ids)
print(right_ids)
print(bottom_ids)
print(left_ids)


masks = compute_perimeterlines(128)

def main():

	fig, ax = plt.subplots()
	fig.show()

	for mask in masks:
		ax.imshow(mask)
		fig.canvas.draw()
		plt.waitforbuttonpress()
		ax.cla()


if __name__ == "__main__":
    main()
