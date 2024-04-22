import socket
import threading
import time
import cv2
import numpy as np
import struct

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
lower_red2 = np.array([170, 50, 50])
upper_red2 = np.array([180, 255, 255])

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
    capture = cv2.VideoCapture(0) # Change 0 On Nvidia To camera Port
    ret,image = capture.read()
    if not ret:
        print("FAILED TO CAPTURE IMAGE")
        return -1
    capture.release()

    blockval = 0 # Tentatively no cube

    # Convert BGR image to HSV
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

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

def detectObstruction():
    # Capture Single Image From Onboard Camera
    capture = cv2.VideoCapture(0) # Change 0 On Nvidia To camera Port
    ret,image = capture.read()
    if not ret:
        print("FAILED TO CAPTURE IMAGE")
        return -1
    capture.release()

     # Load an image in BGR format
    kernel = np.ones((5, 5), np.uint8)
    # Convert BGR to HSV
    image_hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Threshold the HSV image to get only red colors
    mask_gray = cv2.inRange(image_hsv, lower_gray, upper_gray)
    
    mask_gray = cv2.morphologyEx(mask_gray, cv2.MORPH_CLOSE, kernel)
    mask_gray = cv2.morphologyEx(mask_gray, cv2.MORPH_OPEN, kernel)

    # Portion for Displaying Result (Comment out) ########
    #result = cv2.bitwise_and(image, image, mask=mask_gray)

    #cv2.imshow('Result', result)
    #cv2.waitKey(0)
    #cv2.destroyAllWindows()

    #######################################################

    contours_gray, _ = cv2.findContours(mask_gray, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # Loop through All detected Contours 
    for contour in contours_gray:
        area = cv2.contourArea(contour)
        if area > 3000:  # Adjust this threshold as needed to toggle the closeness of the detection
            print(area)
            print("Gray Cube Detected")
            return True

    return False

# Process a Single Maze Frame Image
def processMazeImage():
    color = detect_colored_cube()
    if (color == 0):
        return 0  # No Block Encountered
    block_count[color-1]+= 1
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

# Case Statement for Recieved Messages
switch_dict = {
    1:processMazeImage(),
    2:processObstructionImage(),
    3:processAudio(),
    4:broadcastStop(),
    5:broadcastResume(),
}

# Thread Created Function for Each Heltec's Queries
def HandleHeltec(heltec_socket,heltec_address):
    # Designated Communication Channel For Each Heltec
    while True:
        # Constantly Wait for Recieve, and then send response
        data = heltec_socket.recv(4)
        # Cast Data to Integer
        data = int(data,2)
        #result = switch_dict.get(data,case_default)() # uncomment
        result = 1
        binary_data = result.to_bytes(4,byteorder='little') # Send as Little Endian, as Network Reverses the 
        heltec_socket.sendall(binary_data)
        result = switch_dict.get(data,case_default)() # Result of Respective function Call
       
def InitiateServer():
    # Initial Socket For Accepting devices
    # TCP Server Socket Object
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # WLAN address, subject to change, check on linux with 'ifconfig -a', windows with 'ipconfig'
    server_ip = "192.168.1.155"
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

# Run Nvidia Server
InitiateServer()
