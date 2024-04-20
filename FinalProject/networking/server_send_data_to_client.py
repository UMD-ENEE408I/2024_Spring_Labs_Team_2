# send and receive string to heltec // not sending a string rightnow
import socket
import sys



def run_server():
    # create a socket object
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_ip = "172.20.10.5" 
    port = 8000

    # bind the socket to a specific address and port
    server.bind((server_ip, port))
    # listen for incoming connections
    server.listen(0)
    print(f"Listening on {server_ip}:{port}")

    # accept incoming connections
    client_socket, client_address = server.accept()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

    # receive data from the client
    while True:
      
        msg = "hello client".encode("utf-8") # convert string to bytes
        print("msg is " , msg)
        # convert and send accept response to the client
        client_socket.send(msg)

        response = client_socket.recv(1024)
        response = response.decode("utf-8")

        if response.lower() == "closed":
            break


    # close connection socket with the client
    client_socket.close()
    #print("Connection to client closed")
    # close server socket
    
    server.close()


run_server()
