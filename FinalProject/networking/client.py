# For sending Messages to the Heltec
import socket

def run_client():
    # create a socket object
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_ip = "192.168.1.171"  # replace with the Heltec's Known IP Address
    server_port = 8000  # Arbitraily chosen, must coincide with port chosen on Heltec
    # establish connection with server
    client.connect((server_ip, server_port))
    
    while True:
        # input message and send it to the server
        msg = input("Enter message: ")
        client.send(msg.encode("utf-8")[:1024])

        # receive message from the server
        response = client.recv(1024)
        response = response.decode("utf-8")

        # if server sent us "closed" in the payload, we break out of the loop and close our socket
        if response.lower() == "closed":
            break

        print(f"Received: {response}")

    # close client socket (connection to the server)
    client.close()
    print("Connection to server closed")

run_client()
