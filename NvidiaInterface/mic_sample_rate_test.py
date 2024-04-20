import pyaudio

p = pyaudio.PyAudio()

def print_supported_rates(device_index):
    info = p.get_device_info_by_index(device_index)
    print(f"Device name: {info['name']}")
    try:
        for rate in [8000, 16000, 32000, 44100, 48000, 96000]:
            stream = p.open(format=pyaudio.paInt16, channels=1,
                            input_device_index=device_index,
                            frames_per_buffer=1024, rate=rate, input=True)
            stream.close()
            print(f"Supported sample rate: {rate} Hz")
    except Exception as e:
        print(f"Sample rate {rate} Hz not supported - {e}")

# Replace '24' with the device index you want to check
print_supported_rates(24)
