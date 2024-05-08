# last edited 5_8_24

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

import socket
import threading
import time
import cv2
import struct

def bandpass(data: np.ndarray, edges: list[float], sample_rate: float, poles: int = 5):
    sos = scipy.signal.butter(poles, edges, 'bandpass', fs=sample_rate, output='sos')
    filtered_data = scipy.signal.sosfiltfilt(sos, data)
    return filtered_data

def identify_sound_source() :
    #chunk = 1024  # Record in chunks of 1024 samples will probably need to increase this sinc we increased sample rate
    chunk = 6144 # for NVIDIA
    sample_format = pyaudio.paInt16  # 16 bits per sample
    #channels = 2 # right and left
    channels = 1 #monochannelS
    #fs = 44100  # Record at 44100 samples per second does not appear to be supported
    fs = 48000  # Record at 48000 samples per second only rate that works for NVIDIA
    seconds = 3
    filename1 = "unfiltered_Mic1.wav"  # usbaudio1.0
    filename2 = "unfiltered_Mic2.wav" # 2-usbaudio1.0
    result = 0 

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
                input_device_index= 1) # Microphone1 (2-USBAudio1.0) , Device Index: 3 for laptop, for nvidia its 0



    stream2 = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True,
                input_device_index= 2) # Microphone2 (USBAudio1.0) , Device Index: 1 matches speaker output for laptop and for Nvidia
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
    filtered1 = bandpass(data1, [8000, 10000], Fs1)
    filtered2 = bandpass(data2, [8000, 10000], Fs1)
    

    #calaculate average amplitude RMS
    blockLinearRms1= np.sqrt(np.mean(filtered1**2)) # Linear value between 0 -> 1
    blockLogRms1 = 20 * math.log10(blockLinearRms1) # Decibel (dB value) between 0 dB -> -inf dB
    blockLinearRms2= np.sqrt(np.mean(filtered2**2)) # Linear value between 0 -> 1
    blockLogRms2 = 20 * math.log10(blockLinearRms2) # Decibel (dB value) between 0 dB -> -inf dB

    print("avg rms filtered 1: " +str(blockLinearRms1))
    print("avg rms filtered 2: " +str(blockLinearRms2))

    print("Mic1/Mic2: " +str(blockLinearRms1/blockLinearRms2))
    if blockLinearRms1/blockLinearRms2 > 2 :
            result = 1 # Right toward mic1

    else: 
            result =0 # LEft toward mic2

   # print("Mic1/Mic2: " +str(blockLinearRms1/blockLinearRms2))
    
    return result

# Recieve Constructs: 
# 1/2 = Process Current Image Frame (Color Block Detection), and Return Decision
# 3 = Process Audio, and Return Decision
# 4 = Broadcast Stop Signal to all Current Clients
# 5 = Broadcast Resume Signal to all Current Clients

# Send Constructs
# 0 = No Obstruction Detected, Continue
# 1 = Obstruction Detected, Stop


# Global Data:
block_count = [0,0,0] # Red, Blue, Green
current_clients = [] # List containing all active clients in Tuple Format [client_socket,client_address]

# HSV Color Ranges for Cube Detection
lower_red1 = np.array([0, 50, 50])
upper_red1 = np.array([10, 255, 255]) # Red Colors Wrap Around the HSV Boundaries 
                                      # Solution: Convolve w/ Two Masks
lower_red2 = np.array([175, 50, 50])
upper_red2 = np.array([190, 255, 255])

lower_green = np.array([30, 100, 100])
upper_green = np.array([90, 255, 255])

lower_blue = np.array([100, 50, 50])
upper_blue = np.array([150, 255, 255])

lower_gray = np.array([0, 0, 50])
upper_gray = np.array([179, 1, 255])

# Morphological Kernel
kernel = np.ones((5, 5), np.uint8)

#Image Processing Functions
# Image Processing Code

# Construct Constants
# No cube = 0

def detect_colored_cube():
    i = 0
    cap = cv2.VideoCapture(1)
    while (i < 5): # Delay by 5 Frames to lag startup
        ret, frame = cap.read()
        i = i+1

    # Check if the frame was read successfully
    if not ret:
        print("Error: Could not capture frame.")
        cap.release()
        return

    # Release the camera and close all OpenCV windows
    cap.release()
    cv2.destroyAllWindows()

    # Convert BGR image to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Threshold the image to get binary masks for each color
    mask_red1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(hsv, lower_red2, upper_red2)
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)

    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    # Reduce Noise with Morphological Convolutions
    mask_red = cv2.morphologyEx(mask_red, cv2.MORPH_CLOSE, kernel)
    mask_red = cv2.morphologyEx(mask_red, cv2.MORPH_OPEN, kernel)

    mask_blue = cv2.morphologyEx(mask_blue, cv2.MORPH_CLOSE, kernel)
    mask_blue = cv2.morphologyEx(mask_blue, cv2.MORPH_OPEN, kernel)

    mask_green = cv2.morphologyEx(mask_green, cv2.MORPH_CLOSE, kernel)
    mask_green = cv2.morphologyEx(mask_green, cv2.MORPH_OPEN, kernel)

    # Find contours
    contours_red, _ = cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # Loop through contours
    for contour in contours_red:
        area = cv2.contourArea(contour)
        if area > 1000:  # Adjust this threshold as needed for spatial closeness
            print(area)
            # If significant area, a colored cube is present
            print("Red Cube Detected")
            blockval= 1
    contours_blue, _ = cv2.findContours(mask_blue, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # Loop through contours
    for contour in contours_blue:
        area = cv2.contourArea(contour)
        if area > 1000:  # Adjust this threshold as needed for spaital closeness
            print(area)
            # If significant area, a colored cube is present
            print("Blue Cube Detected")
            blockval= 2
    contours_green, _ = cv2.findContours(mask_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # Loop through contours
    for contour in contours_green:
        area = cv2.contourArea(contour)
        if area > 1000:  # Adjust this threshold as needed for spatial closeness
            print(area)
            # If significant area, a colored cube is present
            print("Green Cube Detected")
            blockval= 3
        
    # No significant colored cube detected
    return blockval
cap = cv2.VideoCapture(0)
def detectObstruction():
    i = 0
    while (i < 5): # Delay by 5 Frames to lag startup
        ret, frame = cap.read()
        i = i+1
    # Check if the frame was read successfully
    if not ret:
        print("Error: Could not capture frame.")
        #cap.release()
        return
    # Release the camera and close all OpenCV windows
    #cap.release()
    cv2.destroyAllWindows()
     # Load an image in BGR format
    kernel = np.ones((5, 5), np.uint8)
    # Convert BGR to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Threshold the HSV image to get only red colors
    #mask_gray = cv2.inRange(hsv, lower_gray, upper_gray)
    
    #mask_gray = cv2.morphologyEx(mask_gray, cv2.MORPH_CLOSE, kernel)
    #mask_gray = cv2.morphologyEx(mask_gray, cv2.MORPH_OPEN, kernel)

    mask_red1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(hsv, lower_red2, upper_red2)
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)

    # Portion for Displaying Result (Comment out) ########
    #result = cv2.bitwise_and(frame, frame, mask=mask_red)

    #cv2.imshow('Result', result)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()

    #######################################################

    contours_gray, _ = cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # Loop through All detected Contours 
    for contour in contours_gray:
        area = cv2.contourArea(contour)
        print(area)
        if area > 1000:  # Adjust this threshold as needed to toggle the closeness of the detection
            print(area)
            return True

    return False

# Process a Single Maze Frame Image
def processMazeImage():
    color = detect_colored_cube()
    if (color == 0):
        return 0  # No Block Encountered
    #block_count[color-1]+= 1
    return 1
# Process Audio and Return Left Turn vs Right turn : Annes Function
def processAudio(): 

    return
# Broadcast To All Current Clients, Stop # May Not Implement
def broadcastStop():
    return
# Broadcast to All Current Clients, Resume
def broadcastResume():
    return
# Process a Single Asteroid-Field Image
def processObstructionImage():
    if detectObstruction():
        return 1
    else:
        return 0

def case_default():
    return

# Thread Created Function for Each Heltec's Queries
def HandleHeltec(heltec_socket,heltec_address):
    # Designated Communication Channel For Each Heltec
    i = 0
    while True:
        # Constantly Wait for Recieve, and then send response
        data = heltec_socket.recv(4)
        binary_string = bin(int.from_bytes(data, byteorder='little'))
        # Get rid of the '0b' prefix from the binary string
        binary_string = binary_string[2:]
        data = int(binary_string,2)
        if (data ==3): 
            result = identify_sound_source()
        if (data == 2):
            result = processObstructionImage()
        if (data == 1):
           result = processObstructionImage()
        
        
        print("Integer Recieved")
        print(data)
        result = processObstructionImage()
        
        #print(result)
        #if(data == 3 or data == 16 or data == 25 or data == 29):
         #   result =1
        #else:
         #   result = 0
        print("Result: ")
        print(result)
        binary_data = result.to_bytes(4,byteorder='little') # Send as Little Endian, as Network Reverses the 
        heltec_socket.sendall(binary_data)
        i = i+1
       
def InitiateServer():
    # Initial Socket For Accepting devices
    # TCP Server Socket Object
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # WLAN address, subject to change, check on linux with 'ifconfig -a', windows with 'ipconfig'
    server_ip = "172.20.10.11"
    port = 8000 # Port arbitrarily chosen, must coincide with the socket port specified on heltec
    # Binding socket
    server.bind((server_ip, port))
    # Waiting for connection
    server.listen(4) # Queue Of Up to four incoming Connections
    # Connection Established

    print(f"Listening on {server_ip}:{port}")
    try:
        while True: # Indefinitely Accept Incoming Clients on port 8000
            client_socket, client_address = server.accept()
            current_clients.append([client_socket,client_address])
            print(f"Accepted connection from {client_address[0]}:{client_address[1]}")
            # Create a Designated Thread for Each incoming Client and Start without 'Join' as to allow new clients to connect
            client_thread = threading.Thread(target=HandleHeltec, args=(client_socket, client_address))
            client_thread.start()
    except KeyboardInterrupt:
        print("Task Complete! Exiting Client Acception")
    finally:
        server.close()

InitiateServer()



