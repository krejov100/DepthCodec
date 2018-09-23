from sklearn.metrics import mean_squared_error
import numpy as np


def peak_signal_to_noise(a, b):
    return (20*np.log10(0xffff)) - (10 * np.log10(mean_squared_error(a, b)))
