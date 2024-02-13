import matplotlib
import matplotlib.pyplot as plt
import scipy as scp
import numpy as np
import cv2

fs = 8000

fs, m1_signal = scp.io.wavfile.read('M1.wav')
fs, m2_signal = scp.io.wavfile.read('M2.wav')
fs, m3_signal = scp.io.wavfile.read('M3.wav')

print("RMS value for M1.wav:", np.sqrt(np.mean((np.square(np.int64(m1_signal))))))
print("RMS value for M2.wav:", np.sqrt(np.mean((np.square(np.int64(m2_signal))))))
print("RMS value for M3.wav:", np.sqrt(np.mean((np.square(np.int64(m3_signal))))))