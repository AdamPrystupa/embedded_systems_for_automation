import socket
SERVER_ADDRESS = 'agrosensor.jumpingcrab.com'
SERVER_PORT = 8051
DATA1 = "GET /swa10-task1.php\r\n\r\n"


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((SERVER_ADDRESS, SERVER_PORT))
    sock.sendall(DATA1.encode('ascii'))
    response = sock.recv(512).decode('ascii').strip()
    print(f'{DATA1.strip()} ==> {response}')
    
