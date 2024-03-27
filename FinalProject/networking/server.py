import socket

def run_server():
    # TCP Server Socket Object
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # WLAN address, subject to change, check on linux with 'ifconfig -a', windows with 'ipconfig'
    server_ip = "192.168.1.169"
    port = 8000 # Port arbitrarily chosen, must coincide with the socket port specified on heltec

    # Binding socket
    server.bind((server_ip, port))
    # Waiting for connection
    server.listen(0)
    # Connection Established
    print(f"Listening on {server_ip}:{port}")
    # accept incoming connections
    client_socket, client_address = server.accept()
    print(f"Accepted connection from {client_address[0]}:{client_address[1]}")

    request = client_socket.recv(1024)
    request = request.decode("utf-8") # convert bytes to string

    print(f"Received: {request}")

    response = "accepted".encode("utf-8")
    client_socket.send(response)

    # close connection socket with the client and close server
    client_socket.close()
    server.close()

run_server()
