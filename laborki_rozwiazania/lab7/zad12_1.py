import socket
from secret import wifi_connect
SERVER_ADDRESS = '10.0.0.198'
SERVER_PORT = 80
DATA1 = ("GET /swa10-task1.php?id=275496 HTTP/1.1\r\n"
         "Host:10.0.0.198\r\n\r\n"
         )

wifi_connect()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    sock.settimeout(10)
    sock.connect((SERVER_ADDRESS, SERVER_PORT))
    sock.sendall(DATA1.encode('ascii'))
    response = sock.recv(1024).decode('ascii')
    print(f'{DATA1.strip()} ==> {response.strip()}')
    f = open('zad12_1.json', 'w')
    f.write(response)
    f.close()
except Exception as e:
    print(f"Inny błąd: {e}")


finally:
    sock.close() 
