import socket
from secret import wifi_connect
SERVER_ADDRESS = '10.0.0.198'
SERVER_PORT = 80
DATA1 = "GET /swa10-task1.php?id=275496\r\n\r\n"

wifi_connect()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    sock.connect((SERVER_ADDRESS, SERVER_PORT))
    sock.sendall(DATA1.encode('ascii'))
    response = sock.recv(512).decode('ascii').strip()
    print(f'{DATA1.strip()} ==> {response}')
    f = open('zad12.json', 'w')
    f.write(response)
    f.close()

finally:
    sock.close() 