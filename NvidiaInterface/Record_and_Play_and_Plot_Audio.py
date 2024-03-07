# record audio from a microphone, play it back and plot a spectrogram and time domain plot
import numpy as np
import matplotlib.pyplot as plt
import pyaudio
import wave
from playsound import playsound
from scipy.io import wavfile


#record audio using pyaudio


chunk = 1024  # Record in chunks of 1024 samples
sample_format = pyaudio.paInt16  # 16 bits per sample
#channels = 2 # right and left
channels = 1 #monochannel
fs = 44100  # Record at 44100 samples per second
seconds = 10
filename = "CruelSummer.wav"

p = pyaudio.PyAudio()  # Create an interface to PortAudio

print('Recording')

stream = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True)

frames = []  # Initialize array to store frames

# Store data in chunks for 3 seconds
for i in range(0, int(fs / chunk * seconds)):
    data = stream.read(chunk)
    frames.append(data)

# Stop and close the stream 
stream.stop_stream()
stream.close()
# Terminate the PortAudio interface
p.terminate()

print('Finished recording')

# Save the recorded data as a WAV file
wf = wave.open(filename, 'wb') 
wf.setnchannels(channels)
wf.setsampwidth(p.get_sample_size(sample_format))
wf.setframerate(fs)
wf.writeframes(b''.join(frames))
wf.close()

# plot specgram
Fs, data = wavfile.read('CruelSummer.wav') #wav file returns the sampled frequency (inherent to that wave file), and the data vector
data=(np.array(data, dtype='int64')) #cast data as int64

print('sample rate1 {} Hz' .format(Fs)) #Fs
length_of_data = len(data)
N = length_of_data 
timestep = 1/Fs #sample time interval
print("length of data {}" .format(length_of_data))
print(data)
time_vector =np.linspace(0,(N-1)*timestep,N) # for plotting purposes

print('playing recording')
print(data.shape) # returns a 2D array(440320,2)
# leftchannel = data[:,0]
# rightchannel = data[:,1]
playsound('CruelSummer.wav') # play recording

plt.figure(1)
plt.specgram(data, Fs)  # wants only 1D so I selected the left channel
plt.title('Spectrogram, Single Channel Output:Taylor Swift Cruel Summer',  
          fontsize = 14, fontweight ='bold') 

plt.figure(2)
plt.plot(time_vector, data)
plt.xlabel('Time (s) ')
plt.ylabel('Amplitude')
plt.title('Single Channel Output: Taylor Swift Cruel Summer') 

# plt.figure(3)
# plt.plot(time_vector,leftchannel)
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.title('recorded wave')

# plt.figure(4)
# plt.plot(time_vector,rightchannel)
# plt.xlabel('time s ')
# plt.ylabel('Amplitude')
# plt.title('right channel')
# # spectrogram

# plt.figure(5)
# plt.specgram(leftchannel, Fs)  # wants only 1D so I selected the left channel
# plt.title('matplotlib.pyplot.specgram() Example\n',  
#           fontsize = 14, fontweight ='bold') 

# plt.figure(6)
# plt.specgram(rightchannel, Fs)  # wants only 1D so I selected the left channel
# plt.title('matplotlib.pyplot.specgram() Example\n',  
#           fontsize = 14, fontweight ='bold') 


  

plt.show()
