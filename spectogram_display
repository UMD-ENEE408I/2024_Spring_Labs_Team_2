import matplotlib.pyplot as plt
import numpy as np
from scipy import signal
from scipy.io import wavfile

# Obtaining a spectogram
sample_rate, samples = wavfile.read('file.wav')
frequencies, times, spectrogram = signal.spectrogram(samples, sample_rate)

plt.plot(np.arange(len(samples)), samples)
plt.ylabel('Amplitude')
plt.xlabel('Sample')
plt.show()

plt.pcolormesh(times, frequencies, np.log(spectrogram))
plt.ylabel('Frequency [Hz]')
plt.xlabel('Time [sec]')
plt.show()
