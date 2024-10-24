import socket
import threading
import os
from datetime import datetime

# 服务器地址和端口
host = '0.0.0.0'
port = 9999

# 存储客户端连接
clients = []

# 锁对象
lock = threading.Lock()

# 服务器日志
def log(message):
    with lock:
        print(f"{datetime.now()}: {message}")

# 处理客户端请求
def handle_client(client_socket, address):
    try:
        log(f"连接来自 {address}")
        while True:
            data = client_socket.recv(1024).decode()
            if not data:
                break
            command, *args = data.split()
            if command == 'LIST':
                response = '\n'.join(os.listdir('.'))
            elif command == 'SEND':
                filename = args[0]
                with open(filename, 'wb') as f:
                    while True:
                        chunk = client_socket.recv(1024)
                        if not chunk:
                            break
                        f.write(chunk)
                response = f"文件 {filename} 接收完毕。"
            elif command == 'GET':
                filename = args[0]
                with open(filename, 'rb') as f:
                    response = f.read()
            else:
                response = "未知命令"
            if command != 'GET':
                client_socket.sendall(response.encode())
        log(f"断开连接 {address}")
    except Exception as e:
        log(f"错误: {e}")
    finally:
        client_socket.close()

# 启动服务器
def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(5)
    log("服务器启动，等待连接...")
    while True:
        client_socket, address = server_socket.accept()
        clients.append(client_socket)
        thread = threading.Thread(target=handle_client, args=(client_socket, address))
        thread.start()

if __name__ == "__main__":
    start_server()