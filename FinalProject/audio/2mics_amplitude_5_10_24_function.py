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
                input_device_index= 2) # Microphone1 (2-USBAudio1.0) , Device Index: 3 for laptop, for nvidia its 0



    stream2 = p.open(format=sample_format,
                channels=channels,
                rate=fs,
                frames_per_buffer=chunk,
                input=True,
                input_device_index= 5) # Microphone2 (USBAudio1.0) , Device Index: 1 matches speaker output for laptop and for Nvidia
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
    if blockLinearRms1/blockLinearRms2 > 4 :
            result = 1 # Right toward mic1

    else: 
            result =0 # LEft toward mic2

   # print("Mic1/Mic2: " +str(blockLinearRms1/blockLinearRms2))
    
    return result
