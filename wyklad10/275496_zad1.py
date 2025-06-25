import socket
SERVER_ADDRESS = 'agrosensor.jumpingcrab.com'
SERVER_PORT = 8052
DATA1 = "login 123456\r\n"
DATA2 = "cmd1\r\n"
DATA3 = "quit\r\n"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((SERVER_ADDRESS, SERVER_PORT))
    sock.sendall(DATA1.encode('ascii'))
    response = sock.recv(512).decode('ascii').strip()
    print(f'{DATA1.strip()} ==> {response}')
    
    sock.sendall(DATA2.encode('ascii'))
    response = sock.recv(512).decode('ascii').strip()
    print(f'{DATA2.strip()} ==> {response}')
    
    sock.sendall(DATA3.encode('ascii'))
    response = sock.recv(512).decode('ascii').strip()
    print(f'{DATA3.strip()} ==> {response}')