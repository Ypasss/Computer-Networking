#include <iostream>  // 包含标准输入输出流库，用于控制台输入输出
#include <Winsock2.h>  // 包含Winsock 2.2的库，用于网络编程
#include <ws2tcpip.h>  // 包含网络编程中使用的额外函数和类型
#include <map>  // 包含标准模板库中的map容器，用于存储客户端计数
#include <cstring>  // 包含C风格的字符串处理函数

#pragma comment(lib, "Ws2_32.lib")  // 指示链接器链接Ws2_32.lib库，这个库包含Winsock函数的实现

int main() {
    WSADATA wsaData;  // 用于存储WSAStartup函数的信息
    SOCKET serverSocket = INVALID_SOCKET;  // 初始化服务器套接字为INVALID_SOCKET，表示未创建套接字
    struct sockaddr_in server, client;  // sockaddr_in结构用于存储服务器和客户端的地址信息
    int recvSize;  // 用于存储接收到的数据的大小
    char recvBuff[1024];  // 存储接收到的数据
    std::map<std::string, int> clientCounter;  // 用于统计每个客户端发送的数据包数量

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "Winsock init failed. Error: " << WSAGetLastError() << std::endl;
        return 1;  // 如果初始化失败，输出错误并退出程序
    }

    // 创建服务器套接字
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {  // 如果套接字创建失败
        std::cout << "socket() error: " << WSAGetLastError() << std::endl;
        WSACleanup();  // 清理Winsock环境
        return 1;  // 退出程序
    }

    // 填充服务器地址信息
    server.sin_family = AF_INET;  // 指定地址族为IPv4
    server.sin_addr.s_addr = INADDR_ANY;  // 指定地址为任意（本机地址）
    server.sin_port = htons(12345);  // 指定端口号，并将端口号转换为网络字节序

    // 绑定套接字到指定的地址和端口
    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cout << "bind() error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);  // 关闭套接字
        WSACleanup();  // 清理Winsock环境
        return 1;  // 退出程序
    }

    std::cout << "UDP server up and listening" << std::endl;

    // 服务器主循环，不断接收客户端发送的数据
    while (true) {
        recvSize = sizeof(client);  // 设置接收缓冲区大小
        int cBytes = recvfrom(serverSocket, recvBuff, sizeof(recvBuff), 0, (struct sockaddr *)&client, &recvSize);  // 接收数据
        if (cBytes > 0) {
            recvBuff[cBytes] = '\0'; // 确保字符串以null结尾
            std::cout << "Received message from " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << std::endl;  // 输出客户端地址
            std::cout << "Message: " << recvBuff << std::endl;  // 输出接收到的消息

            // 构建客户端标识字符串
            std::string clientKey = std::string(inet_ntoa(client.sin_addr));
            // 更新客户端发送的消息数量
            clientCounter[clientKey]++;

            // 输出客户端发送的消息数量
            std::cout << "Client " << clientKey << " has sent " << clientCounter[clientKey] << " messages" << std::endl;
            std::cout << "------" << std::endl;
        }
    }

    // 关闭套接字
    closesocket(serverSocket);
    // 清理Winsock环境
    WSACleanup();
    return 0;  // 正常退出程序
}