from gradiant import *
import numpy as np
import matplotlib.pyplot as plt


def main():
	n = 100
	noise = np.random.normal(20, 10,(n,n))
	grad = make_gradiant(n,0,100,100) + noise
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
