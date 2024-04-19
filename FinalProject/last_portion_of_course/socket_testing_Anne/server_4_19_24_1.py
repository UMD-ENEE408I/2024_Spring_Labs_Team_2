#from socket import * doesnt require socket.socket but doesnt have gethostname function
import socket 
import threading # helpful if we want to know the number of active connections to our server
import sys

HEADER = 1024
PORT  =  8000 # above 4000 ae unused ports in computer
SERVER = "172.20.10.5" # ip address server is listening on, we can see this in ipconfig 
#SERVER = "127.0.0.1" local host only use if you are running both client and server on same device
#SERVER = socket.gethostbyname(socket.gethostname()) # gets the local IP address of this computer will NOT match ipconfig above
#print(socket.gethostname()) #gives us name of our computer desktop-f84p45

ADDR = (SERVER, PORT) #has to be a touple, with ip address of sever and port
FORMAT = 'utf-8'
DISCONNECT_MESSAGE = "!DISCONNECT"

serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #create the serversocket, what type of ip address are we looking for in connections AF_INET is ipv4, streaming data through socket
serverSocket.bind(ADDR) # bind the server to the address, so anything that connnets to that address will hit the socket

# function to accept message from client and decode it, as well as close socket
def handle_client (connectionSocket, addr):
    print(f"[NEW CONNECTION] {addr} connected.")

# 
    connected = True
    while connected:
        msg_length= connectionSocket.recv(1024)
        msg_length = msg_length.decode("utf-8")
        #msg_length = connectionSocket.recv(HEADER).decode(FORMAT) # how many bytes do we want to recieve from client , and decode the byte format into string using utf8
        # if msg_length: 
        #     print("variable type" ,type(msg_length))
        #     #msg_length = float(msg_length)con
        #     msg_length = bytes(msg_length, 'utf-8')
        #     print("variable type" ,type(msg_length))
        #     print("size of length of message", sys.getsizeof(msg_length)) #length of message is 47bytes
        #     msg_length = int.from_bytes(msg_length, "big")
        #     print("variable type" ,type(msg_length))
        #     #msg_length = int(msg_length) #convet into integer 
        #     msg = connectionSocket.recv(msg_length).decode(FORMAT) 
            
            # if msg == DISCONNECT_MESSAGE:
            #     connected = False
            # print(f"[{addr}] {msg}")

        if msg_length.lower() ==  DISCONNECT_MESSAGE:
            connectionSocket.send ("closed".encode("utf-8"))
            break
        print(f"Received: {msg_length}")

    connectionSocket.close() 
    



# function to listen to client and determine number of connections
def start():
    serverSocket.listen()
    print(f"[LISTENING] Server is listening on {SERVER}")
    while True:
        connectionSocket, addr = serverSocket.accept() # waits for new connection to server, ip address and port is saved  as a new socket in teh server called connection socket dedicatd to this particular client
       # handle_client(connectionSocket,addr)
        thread = threading.Thread(target = handle_client, args = (connectionSocket, addr))
        thread.start()
        print(f"[ACTIVE CONNECTIONS] {threading.active_count() - 1}")

print("starting:server is starting")
start() #listing to connections and passing connections to handle client