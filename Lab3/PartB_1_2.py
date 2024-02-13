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


def convolution(input_signal, kernel):
    input_len = len(input_signal)
    kernel_len = len(kernel)
    output_len = input_len + kernel_len - 1
    output_signal = [0] * output_len

    for i in range(output_len):
        for j in range(kernel_len):
            if i - j >= 0 and i - j < input_len:
                output_signal[i] += (input_signal[i - j] * kernel[j])
            
        
    return output_signal

convolved = scp.signal.convolve(m1_signal,m2_signal)
maxindex = np.argmax(convolved)
#Index of Maximum Value
print(maxindex)
#Time Delay(Index/sampling rate)
tdelay1 = maxindex/sr1)
print(tdelay1)
