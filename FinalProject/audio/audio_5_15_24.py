# last edited 4_29_24
# filters a signal using bandpass, correlates filtered signals. It also determines RMS of filtered signals to determine avg amplitude.
# the correlation is also performed on the center region of the signals which has produced better results 
import matplotlib.pyplot as plt
import numpy as np
import math
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
import scipy.signal 
from scipy.signal import find_peaks

def bandpass(data: np.ndarray, edges: list[float], sample_rate: float, poles: int = 5):
    sos = scipy.signal.butter(poles, edges, 'bandpass', fs=sample_rate, output='sos')
    filtered_data = scipy.signal.sosfiltfilt(sos, data)
    return filtered_data


#chunk = 1024  # Record in chunks of 1024 samples will probably need to increase this sinc we increased sample rate
chunk = 6144 # for NVIDIA
sample_format = pyaudio.paInt16  # 16 bits per sample
#channels = 2 # right and left
channels = 1 #monochannelS
#fs = 44100  # Record at 44100 samples per second does not appear to be supported
fs = 48000  # Record at 48000 samples per second only rate that works for NVIDIA
seconds = 5
filename1 = "unfiltered_Mic1.wav"  # usbaudio1.0
filename2 = "unfiltered_Mic2.wav" # 2-usbaudio1.0

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
                input=True)
                #input_device_index= 3) # Microphone1 (2-USBAudio1.0) , Device Index: 3 for laptop, for nvidia its 0



stream2 = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True,
                input_device_index= 3) # Microphone2 (USBAudio1.0) , Device Index: 1 matches speaker output for laptop and for Nvidia
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


# read unflitered data that is stored in wav file, necessary step for correlation, otherwise errror involving byte conversion occurs
Fs1, data1 = wavfile.read('unfiltered_Mic1.wav') #wav file returns the sampled frequency (inherent to that wave file), and the data vector
data1=(np.array(data1, dtype='int64')) #cast data as int64 
Fs2, data2 = wavfile.read('unfiltered_Mic2.wav') #wav file returns the sampled frequency (inherent to that wave file), and the data vector
data2=(np.array(data2, dtype='int64'))

 
# filter data1 and data2 using bandpass filter

# filtered1 = bandpass(data1, [100, 300], Fs1)
# filtered2 = bandpass(data2, [100, 300], Fs1)
filtered1 = bandpass(data1, [8500, 9500], Fs1)
filtered2 = bandpass(data2, [8000, 10000], Fs1)
 
# writing filtered signal to wavefile
filteredaudio1= np.int16(filtered1.real) #to be able to write to wave file we need to use .real
write("filtered1.wav", Fs1,filteredaudio1)
filteredaudio2= np.int16(filtered2.real) #to be able to write to wave file we need to use .real
write("filtered2.wav", Fs2,filteredaudio2)

# # Using Built-in Correlate Function to correlate filtered signals
# corr1 = (signal.correlate(filtered1, filtered2))
# print("length of corr", len(corr1))
# lags1 = signal.correlation_lags(len(data1), len(data2))
# #print("lags", lags)

# lag1 = lags1[np.argmax(corr1)]
# index_max1= np.argmax(corr1) # argmax returns index of max
# print("lag1 full signal", lag1) 
# max1 = np.max(corr1)
# print("amplitude of max ", max1)
# print("sample number of maximum correlation is", index_max1)

# correlation of just middle second of signal
# reduced_Sig1 = filtered1[60000:80000]
# reduced_Sig2 = filtered2[60000:80000]

# # Using Built-in Correlate Function to correlate filtered signals
# corr2 = (signal.correlate(reduced_Sig1, reduced_Sig2))
# print("length of corr", len(corr2))
# lags2 = signal.correlation_lags(len(reduced_Sig1), len(reduced_Sig2))
# #print("lags", lags) #
# #signal.correlation_lags Returns an array containing cross-correlation lag/displacement indices. 
# # Indices can be indexed with the np.argmax of the correlation to return the lag/displacement.
# print("lags", lags2)
# lag2 = lags2[np.argmax(corr2)] #index of max correlation into lags2 will give you lag value at that index
# index_max2= np.argmax(corr2) # argmax returns index of max
# print("lag1 full signal", lag2) 
# max2 = np.max(corr2)
# print("amplitude of max ", max2)
# print("sample number of maximum correlation is", index_max2)

# print('shape of array :', corr2.shape)

# plt.figure(12)
# peak_indices, _ = find_peaks(corr2)  # why only positive?? from 0 to 80000, i need them to go negative, becaus
# # graph is of lag array which can go negative
# peak_index=lags2[60000]
# print("peak index at -100: ", peak_index)
# print(" peak inices ", peak_indices)
# print("The variable, basics is of type:", type(peak_indices))
# print("The variable, basics is of type:", type(corr2))
# #plt.plot(corr2)
# plt.plot(peak_indices, corr2[peak_indices], "xr")
# # plt.show()




#calaculate average amplitude RMS
blockLinearRms1= np.sqrt(np.mean(filtered1**2)) # Linear value between 0 -> 1
#blockLogRms1 = 20 * math.log10(blockLinearRms1) # Decibel (dB value) between 0 dB -> -inf dB
blockLinearRms2= np.sqrt(np.mean(filtered2**2)) # Linear value between 0 -> 1
# blockLogRms2 = 20 * math.log10(blockLinearRms2) # Decibel (dB value) between 0 dB -> -inf dB

print("avg rms filtered 1: " +str(blockLinearRms1))
print("avg rms filtered 2: " +str(blockLinearRms2))

if blockLinearRms1/blockLinearRms2 > 2 :
        result = 1 # Right

else: 
        result =0 # LEft

print("Mic1/Mic2: " +str(blockLinearRms1/blockLinearRms2))


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
print("time vector 2 {}", len(time_vector2))
print("time vector 1 {}", len(time_vector1))
freqstep2 = Fs2/N2 #freq interval
freq_vector2 =np.linspace(0,(N2-1)*freqstep2,N2) # equivalent to freq = np.fft.fftfreq(N,d=timestep)

# print("length of reduced sig {}", len(reduced_Sig1))
# N3 = len(reduced_Sig1) 
# time_vector3 =np.linspace(0,(N3-1)*timestep2,N3)
# print("length of timevector3 {}", len(time_vector3))

# apply fft to unfiltered data to transform to frequency domain for fft plots
Data_fourier1=np.fft.fft(data1) # D will be a series of complex numbers 
D_mag1 = np.abs(Data_fourier1)/N1 # we want the magntidue and normalize it 
Data_fourier2=np.fft.fft(data2) # D will be a series of complex numbers 
D_mag2 = np.abs(Data_fourier2)/N2 # we want the magntidue and normalize it



# plotting 

plt.figure(1)
plt.subplot(2,1,1)
plt.plot(freq_vector1,D_mag1) # unfiltered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('unfiltered freq output data1')
plt.subplot(2,1,2)
plt.plot(freq_vector2,D_mag2) # unfiltered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('unfiltered freq output data2')
plt.tight_layout()

plt.figure(2)
plt.subplot(2,1,1)
plt.plot(time_vector1,filtered1 )
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 1'  + 'RMS1: '+ str(blockLinearRms1))
plt.subplot(2,1,2)
plt.plot(time_vector2,filtered2)
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 2'  + 'RMS2: '+ str(blockLinearRms2))
plt.tight_layout()


# plt.figure(2)
# plt.subplot(2,1,1)
# plt.specgram(data1, Fs1)  # wants only 1D so I selected the left channel
# plt.title('Spectrogram, Single Channel Output:MIC1 unfiltered',  
#           fontsize = 14, fontweight ='bold') 
# plt.subplot(2,1,2)
# plt.specgram(data2, Fs2)  # wants only 1D so I selected the left channel
# plt.title('Spectrogram, Single Channel Output:MIC1 unfiltered',  
#           fontsize = 14, fontweight ='bold') 
# plt.tight_layout()

# plt.figure(3) # compare unfiltered output to same time points as filtered value
# plt.subplot(2,1,1)
# xmin = 0
# xmax = 0.01
# plt.plot(time_vector3,data1[60000:80000])
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.xlim([xmin, xmax])
# plt.title('unfiltered output 1')
# plt.subplot(2,1,2)
# plt.plot(time_vector3,data2[60000:80000])
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.title('unfiltered output 2')
# plt.xlim([xmin, xmax])
# plt.tight_layout()



plt.figure(3)
plt.subplot(2,1,1)
output_fourier1=np.fft.fft(filtered1) # output will be a series of complex numbers 
output_mag1 = np.abs(output_fourier1)/N1 # we want the magntidue and normlaize it 
plt.plot(freq_vector1,output_mag1) # filtered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('filtered output data1')
plt.subplot(2,1,2)
output_fourier2=np.fft.fft(filtered2) # output will be a series of complex numbers 
output_mag2 = np.abs(output_fourier2)/N2 # we want the magntidue and normlaize it 
plt.plot(freq_vector2,output_mag2) # filtered freq
plt.xlabel('freq ')
plt.ylabel('Amplitude')
plt.title('filtered output data2')
plt.tight_layout()

# plt.figure(5)
# plt.subplot(2,1,1)
# plt.specgram(filtered1, Fs=Fs1)  # wants only 1D so I selected the left channel
# plt.title('Spectrogram, Single Channel Output:MIC1 filtered',  
#           fontsize = 14, fontweight ='bold') 
# plt.xlabel('time s')
# plt.ylabel('Freq Hz')
# plt.subplot(2,1,2)
# plt.specgram(filtered2, Fs =Fs2)  # wants only 1D so I selected the left channel
# plt.title('Spectrogram, Single Channel Output:MIC2 filtered',  
#           fontsize = 14, fontweight ='bold') 
# plt.xlabel('time s')
# plt.ylabel('Freq Hz')
# plt.tight_layout()

plt.figure(4)
plt.subplot(2,1,1)
plt.plot(time_vector1,filtered1 )
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 1'  + 'RMS1: '+ str(blockLinearRms1))
plt.subplot(2,1,2)
plt.plot(time_vector2,filtered2)
plt.xlabel('time s ')
plt.ylabel('Amplitude')
plt.title('filtered output 2'  + 'RMS2: '+ str(blockLinearRms2))
plt.tight_layout()


# plt.figure(7)
# plt.plot(lags1,corr1)
# plt.title('Cross- correlated signal filtered ' + 'lag: '+ str(lag1))
# plt.xlabel('lag')

# plot time and samples of filtered data to determine where to cut the arrays
# plt.figure(8)
# plt.subplot(3,1,1)
# plt.plot(time_vector1,filtered1)
# plt.xlabel('time(s) ')
# plt.ylabel('Amplitude')
# plt.title('filtered output 1'  + 'RMS1: '+ str(blockLinearRms1))
# plt.subplot(3,1,2)
# plt.plot(time_vector2,filtered2)
# plt.xlabel('time(s)')
# plt.ylabel('Amplitude')
# plt.title('filtered output 2' + 'RMS2: '+ str(blockLinearRms2))
# plt.subplot(3,1,3)
# plt.plot(lags1,corr1)
# plt.title('Cross- correlated filtered signal and ' + 'lag: '+ str(lag1))
# plt.xlabel('lag')
# plt.tight_layout()

# plt.figure(9)
# plt.subplot(4,1,1)
# plt.plot(time_vector1,filtered1)
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.title('filtered output 1')
# plt.subplot(4,1,2)
# plt.plot(time_vector2,filtered2)
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.title('filtered output 2')
# plt.subplot(4,1,3)
# plt.plot(filtered1)
# plt.xlabel('samples ')
# plt.ylabel('Amplitude')
# plt.title('filtered output 1')
# plt.subplot(4,1,4)
# plt.plot(filtered2)
# plt.xlabel('samples')
# plt.ylabel('Amplitude')
# plt.title('filtered output 2')
# plt.tight_layout()

#plotting correlation and filtered data for only center 1 second
# plt.figure(10)
# plt.plot(lags2,corr2)
# plt.title('Cross- correlated signal 1second filtered and ' + 'lag: '+ str(lag2))
# plt.xlabel('lag')



# plt.figure(11)
# plt.subplot(3,1,1)
# # xmin = 0.2050
# # xmax = 0.2175
# xmin = 0
# xmax = 0.01
# plt.plot(time_vector3, reduced_Sig1)
# plt.xlabel('time(s) ')
# plt.ylabel('Amplitude')
# plt.title('filtered output 1')
# plt.xlim([xmin, xmax])
# plt.subplot(3,1,2)
# plt.plot(time_vector3, reduced_Sig2)
# plt.xlabel('time(s)')
# plt.ylabel('Amplitude')
# plt.title('filtered output 2')
# plt.xlim([xmin, xmax])
# plt.subplot(3,1,3)
# plt.plot(lags2,corr2)
# plt.title('Cross- correlated signal 1second filtered and ' + 'lag: '+ str(lag2) + ' Sample rate: ' + str(Fs1))
# plt.xlabel('lag')
# plt.xlim([-300, 300])


# #plt.set_ylim([ymin, ymax])
# plt.tight_layout()

plt.show()
