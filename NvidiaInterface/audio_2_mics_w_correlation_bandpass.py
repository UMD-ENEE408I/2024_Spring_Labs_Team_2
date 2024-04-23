#implemented bandpass filter, this program pics up audio from two seperate mics, filters them, takes the fft and plots.

import matplotlib.pyplot as plt
import numpy as np
import pyaudio
import wave
from scipy.fftpack import fft,fftfreq,ifft
from scipy import signal
from scipy.io import wavfile
from scipy.io.wavfile import write
from scipy.signal import freqz
from scipy.signal import butter, lfilter
from scipy.io import wavfile
from scipy.io.wavfile import write

def butter_bandpass(lowcut, highcut, fs, order=5):
    return butter(order, [lowcut, highcut], fs=fs, btype='band')

def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

    # Sample rate and desired cutoff frequencies (in Hz).
lowcut = 8000
highcut = 11000

#chunk = 1024  # Record in chunks of 1024 samples will probably need to increase this sinc we increased sample rate
chunk = 6144
sample_format = pyaudio.paInt16  # 16 bits per sample
#channels = 2 # right and left
channels = 1 #monochannelS
#fs = 44100  # Record at 44100 samples per second does not appear to be supported
fs = 48000  # Record at 48000 samples per second only rate that works for NVIDIA
seconds = 4
filename1 = "Mic1.wav"
filename2 = "Mic2.wav"

p = pyaudio.PyAudio()  # Create an interface to PortAudio

devices = p.get_device_count()

# Iterate through all devices
for i in range(devices):
   # Get the device info
   device_info = p.get_device_info_by_index(i)
   # Check if this device is a microphone (an input device)
   if device_info.get('maxInputChannels') > 0:
      print(f"Microphone: {device_info.get('name')} , Device Index: {device_info.get('index')}")

print('Recording')

stream1 = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True,
                input_device_index= 2) # Microphone (USBAudio1.0) , Device Index: 3 for laptop, for nvidia its 0



stream2 = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True,
                input_device_index= 1) # Microphone (2- USBAudio1.0) , Device Index: 1 matches speaker output for laptop and for Nvidia
frames1 = []  # Initialize array to store frames
frames2 = []


for i in range(0, int(fs / chunk * seconds)):
    data1 = stream1.read(chunk)
    frames1.append(data1)
    data2 = stream2.read(chunk)
    frames2.append(data2) 


stream1.stop_stream()
stream2.stop_stream()
stream1.close()
stream2.close()

# Terminate the PortAudio interface
p.terminate()

print('Finished recording')

# Save the recorded data as a WAV file
wf1 = wave.open(filename1, 'wb') 
wf1.setnchannels(channels)
wf1.setsampwidth(p.get_sample_size(sample_format))
wf1.setframerate(fs)
wf1.writeframes(b''.join(frames1))
wf1.close()

# Save the recorded data as a WAV file
wf2 = wave.open(filename2, 'wb') 
wf2.setnchannels(channels)
wf2.setsampwidth(p.get_sample_size(sample_format))
wf2.setframerate(fs)
wf2.writeframes(b''.join(frames2))
wf2.close()


# read unflitered data
Fs1, data1 = wavfile.read('Mic1.wav') #wav file returns the sampled frequency (inherent to that wave file), and the data vector
data1=(np.array(data1, dtype='int64')) #cast data as int64 
Fs2, data2 = wavfile.read('Mic2.wav') #wav file returns the sampled frequency (inherent to that wave file), and the data vector
data2=(np.array(data2, dtype='int64'))

# Data 1 create time and frequency vectors for plotting
print('sample rate1 {} Hz' .format(Fs1)) #Fs is sampling frequency that is provided by the read function
max_time = data1.size/Fs1 #number samples/frequency gives you time for total signal
timestep1 = 1/Fs1 #sample time interval
length_of_data1 = len(data1)
print("length of data {}" .format(length_of_data1))
N1 = length_of_data1  #572414 samples
time_vector1 =np.linspace(0,(N1-1)*timestep1,N1) #from zero to length of time, for N spaced intervals
freqstep1 = Fs1/N1 #freq interval
freq_vector1 =np.linspace(0,(N1-1)*freqstep1,N1) # equivalent to freq = np.fft.fftfreq(N,d=timestep)

# Data 2 create time and frequency vectors for plotting
print('sample rate1 {} Hz' .format(Fs2)) #Fs is sampling frequency that is provided by the read function
max_time = data2.size/Fs2 #number samples/frequency gives you time for total signal
timestep2 = 1/Fs2 #sample time interval
length_of_data2 = len(data2)
print("length of data {}" .format(length_of_data2))
N2 = length_of_data2  
time_vector2 =np.linspace(0,(N2-1)*timestep2,N2) #from zero to length of time, for N spaced intervals
freqstep2 = Fs2/N2 #freq interval
freq_vector2 =np.linspace(0,(N2-1)*freqstep2,N2) # equivalent to freq = np.fft.fftfreq(N,d=timestep)

# apply fft to unfiltered data to transform to frequency domain
Data_fourier1=np.fft.fft(data1) # D will be a series of complex numbers 
D_mag1 = np.abs(Data_fourier1)/N1 # we want the magntidue and normalize it 
Data_fourier2=np.fft.fft(data2) # D will be a series of complex numbers 
D_mag2 = np.abs(Data_fourier2)/N2 # we want the magntidue and normalize it 

# filter data1 and data2 
filtered1 = butter_bandpass_filter(data1, lowcut, highcut, Fs1, order=6)
filtered2 = butter_bandpass_filter(data2, lowcut, highcut, Fs2, order=6)

# writing filtered signal to wavefile
filteredaudio1= np.int16(filtered1.real) #to be able to write to wave file we need to use .real
write("filtered1.wav", Fs1,filteredaudio1)
filteredaudio2= np.int16(filtered2.real) #to be able to write to wave file we need to use .real
write("filtered2.wav", Fs2,filteredaudio2)

# Using Built-in Correlate Function to correlate filtered signals
corr = np.absolute(signal.correlate(filtered1, filtered2))
print("length of corr", len(corr))
lags = signal.correlation_lags(len(data1), len(data2))
#print("lags", lags)

lag = lags[np.argmax(corr)]
index_max= np.argmax(corr) # argmax returns index of max
print("max corr", max)
print("lag", lag) 
max1 = np.max(corr)
print("amplitude of max ", max1)
print("sample number of maximum correlation is", index_max)


# plotting 
plt.figure(1)
plt.plot(freq_vector1,D_mag1) # unfiltered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('unfiltered freq output data1')

plt.figure(2)
plt.specgram(data1, Fs1)  # wants only 1D so I selected the left channel
plt.title('Spectrogram, Single Channel Output:MIC1 unfiltered',  
          fontsize = 14, fontweight ='bold') 

plt.figure(3)
plt.plot(freq_vector2,D_mag2) # unfiltered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('unfiltered freq output data2')

plt.figure(4)
plt.specgram(data2, Fs2)  # wants only 1D so I selected the left channel
plt.title('Spectrogram, Single Channel Output:MIC1 unfiltered',  
          fontsize = 14, fontweight ='bold') 


plt.figure(5)
plt.plot(time_vector1,filtered1)
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 1')

plt.figure(6)
output_fourier1=np.fft.fft(filtered1) # output will be a series of complex numbers 
output_mag1 = np.abs(output_fourier1)/N1 # we want the magntidue and normlaize it 
plt.plot(freq_vector1,output_mag1) # filtered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('filtered output data1')

plt.figure(7)
plt.specgram(filtered1, Fs1)  # wants only 1D so I selected the left channel
plt.title('Spectrogram, Single Channel Output:MIC1 filtered',  
          fontsize = 14, fontweight ='bold') 

plt.figure(8)
plt.plot(time_vector2,filtered2)
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 2')

plt.figure(9)
output_fourier2=np.fft.fft(filtered2) # output will be a series of complex numbers 
output_mag2 = np.abs(output_fourier2)/N2 # we want the magntidue and normlaize it 
plt.plot(freq_vector2,output_mag2) # filtered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('filtered output data2')

plt.figure(10)
plt.specgram(filtered2, Fs2)  # wants only 1D so I selected the left channel
plt.title('Spectrogram, Single Channel Output:MIC2 filtered',  
          fontsize = 14, fontweight ='bold') 


# plot lag and corrleation of filtered signals
#plt.figure(9)
fig, (ax_orig, ax_noise, ax_corr) = plt.subplots(3, 1, figsize=(4.8, 4.8))
ax_orig.plot(filtered1)
ax_orig.set_title('data1 MIC1 filtered')
ax_orig.set_xlabel('Sample Number')
ax_noise.plot(filtered2)
ax_noise.set_title('data2 MIC2 filtered')
ax_noise.set_xlabel('Sample Number')
ax_corr.plot(lags, corr)
ax_corr.set_title('Cross-correlated signal')
ax_corr.set_xlabel('Lag')
ax_orig.margins(0, 0.1)
ax_noise.margins(0, 0.1)
ax_corr.margins(0, 0.1)
fig.tight_layout()

plt.show()