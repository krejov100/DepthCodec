from sklearn import metrics
import numpy as np
def mse(a, b):
    return metrics.mean_squared_error(a,b)


def psnr(a, b):
    return (20*np.log10(0xffff)) - (10 * np.log10(mse(a, b)))
