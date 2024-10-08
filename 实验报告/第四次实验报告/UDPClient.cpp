#include <iostream>  // 包含标准输入输出流库，用于控制台输入输出
#include <Winsock2.h>  // 包含Winsock 2.2的库，用于网络编程
#include <ws2tcpip.h>  // 包含网络编程中使用的额外函数和类型
#include <cstring>  // 包含C风格的字符串处理函数

#pragma comment(lib, "Ws2_32.lib")  // 指示链接器链接Ws2_32.lib库，这个库包含Winsock函数的实现

int main() {
    WSADATA wsaData;  // 用于存储WSAStartup函数的信息
    SOCKET clientSocket = INVALID_SOCKET;  // 初始化客户端套接字为INVALID_SOCKET，表示未创建套接字
    struct addrinfo hints, *serverInfo;  // addrinfo结构用于存储服务信息
    int K, result;  // K用于存储要发送的消息数量，result用于存储函数调用的返回值
    char message[1024];  // 存储要发送的消息

    // 初始化Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "Winsock init failed. Error: " << WSAGetLastError() << std::endl;
        return 1;  // 如果初始化失败，输出错误并退出程序
    }

    // 设置hints结构，用于后续的getaddrinfo调用
    ZeroMemory(&hints, sizeof(hints));  // 将hints结构清零
    hints.ai_family = AF_INET;  // 指定IPv4地址族
    hints.ai_socktype = SOCK_DGRAM;  // 指定套接字类型为数据报（UDP）
    hints.ai_protocol = IPPROTO_UDP;  // 指定协议为UDP

    // 用户输入服务器的IP地址和端口号
    std::cout << "Enter server IP address: ";
    std::string serverIP;
    std::cin >> serverIP;

    std::cout << "Enter server port: ";
    int serverPort;
    std::cin >> serverPort;

    // 用户输入要发送的消息数量
    std::cout << "Enter number of messages to send (K): ";
    std::cin >> K;

    // 使用getaddrinfo获取服务器的地址信息
    result = getaddrinfo(serverIP.c_str(), std::to_string(serverPort).c_str(), &hints, &serverInfo);
    if (result != 0) {  // 如果getaddrinfo调用失败
        std::cout << "getaddrinfo() error: " << result << std::endl;
        WSACleanup();  // 清理Winsock环境
        return 1;  // 退出程序
    }

    // 创建客户端套接字
    clientSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {  // 如果套接字创建失败
        std::cout << "socket() error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(serverInfo);  // 释放addrinfo结构
        WSACleanup();  // 清理Winsock环境
        return 1;  // 退出程序
    }

    // 用户输入要发送的消息内容
    std::cout << "Enter message to send: ";
    std::cin.getline(message, 1024);  // 使用getline读取一行输入，包括空格

    // 发送K条消息到服务器
    for (int i = 0; i < K; i++) {
        sendto(clientSocket, message, strlen(message), 0, serverInfo->ai_addr, serverInfo->ai_addrlen);
    }

    // 输出完成消息
    std::cout << "Finished sending messages" << std::endl;

    // 清理资源
    freeaddrinfo(serverInfo);  // 释放addrinfo结构
    closesocket(clientSocket);  // 关闭套接字
    WSACleanup();  // 清理Winsock环境
    return 0;  // 正常退出程序
}