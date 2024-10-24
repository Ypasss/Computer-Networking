import socket

# 服务器的IP地址和端口号
HOST = '172.26.117.144'
PORT = 65432

# 创建一个socket对象
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 连接到服务器
client_socket.connect((HOST, PORT))

def list_files():
    """
    发送LIST命令到服务器，接收并打印服务器当前目录下的文件列表。
    """
    client_socket.sendall(b'LIST')  # 发送LIST命令
    data = client_socket.recv(1024)  # 接收服务器响应的数据
    print('Files on server:')
    print(data.decode().strip())  # 打印文件列表

def upload_file(filename):
    """
    上传指定的文件到服务器。
    参数:
    filename: 要上传的文件名。
    """
    with open(filename, 'rb') as f:  # 以二进制读取模式打开文件
        client_socket.sendall(f'UPLOAD {filename}'.encode())  # 发送UPLOAD命令和文件名
        data = f.read(1024)  # 读取文件的1024字节数据
        client_socket.sendall(data)  # 发送文件数据到服务器
    response = client_socket.recv(1024)  # 接收服务器的响应
    print(response.decode().strip())  # 打印服务器响应的消息

def download_file(filename):
    """
    从服务器下载指定的文件。
    参数:
    filename: 要下载的文件名。
    """
    client_socket.sendall(f'DOWNLOAD {filename}'.encode())  # 发送DOWNLOAD命令和文件名
    data = b''
    part = client_socket.recv(1024)  # 接收服务器发送的文件数据
    data += part  # 将接收到的数据追加到data变量
    with open(filename, 'wb') as f:  # 以二进制写入模式打开文件
        f.write(data)  # 将接收到的数据写入文件
    print(f"Downloaded {filename}")  # 打印下载成功的消息

def main():
    """
    主函数，用于接收用户输入的命令并执行相应的操作。
    """
    while True:
        command = input("Enter command (LIST/UPLOAD <file>/DOWNLOAD <file>): ")
        if command == 'LIST':
            list_files()
        elif command.startswith('UPLOAD '):
            filename = command.split(' ', 1)[1]  # 获取文件名
            upload_file(filename)
        elif command.startswith('DOWNLOAD '):
            filename = command.split(' ', 1)[1]  # 获取文件名
            download_file(filename)
        else:
            print('Unknown command')  # 如果输入的命令未知，打印提示信息

if __name__ == "__main__":
    main()
