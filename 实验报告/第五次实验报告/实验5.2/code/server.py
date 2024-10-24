import socket
import os
import threading

# 服务器要绑定的IP地址和端口号
HOST = '172.26.117.144'
PORT = 65432

# 创建一个socket对象
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 绑定IP地址和端口号
server_socket.bind((HOST, PORT))

# 开始监听连接请求
server_socket.listen()

def handle_client(client_socket):
    """
    处理客户端连接的函数。
    参数:
    client_socket: 客户端的socket对象。
    """
    print(f"Connection established with {client_socket.getpeername()}")
    try:
        while True:
            # 接收客户端发送的命令
            command = client_socket.recv(1024).decode()
            if not command:
                # 如果没有接收到命令，说明客户端断开了连接
                break
            print(f"Received command: {command}")
            
            # 处理LIST命令，返回当前目录下的文件列表
            if command == 'LIST':
                files = '\n'.join(os.listdir('.'))
                client_socket.sendall(files.encode() + b'\n')
            
            # 处理UPLOAD命令，接收客户端上传的文件
            elif command.startswith('UPLOAD '):
                filename = command.split(' ', 1)[1]
                with open(filename, 'wb') as f:
                    data = client_socket.recv(1024)
                    f.write(data)
                client_socket.sendall(b'Upload successful\n')
                print(f"File {filename} uploaded successfully.")
            
            # 处理DOWNLOAD命令，发送文件给客户端
            elif command.startswith('DOWNLOAD '):
                filename = command.split(' ', 1)[1]
                try:
                    with open(filename, 'rb') as f:
                        file_data = f.read()
                        client_socket.sendall(file_data)
                except FileNotFoundError:
                    client_socket.sendall(b'File not found\nEOF'.encode())
            
            # 如果命令不是LIST、UPLOAD或DOWNLOAD，返回未知命令
            else:
                client_socket.sendall(b'Unknown command\nEOF')
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        # 关闭客户端连接
        client_socket.close()
        print(f"Connection closed with {client_socket.getpeername()}")

def main():
    """
    主函数，启动服务器。
    """
    print("Server is running...")
    while True:
        # 接受客户端的连接请求
        client_socket, addr = server_socket.accept()
        # 为每个客户端创建一个新的线程来处理请求
        threading.Thread(target=handle_client, args=(client_socket,)).start()

if __name__ == "__main__":
    main()
