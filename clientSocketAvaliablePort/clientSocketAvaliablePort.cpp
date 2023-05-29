#include <iostream>
#include <cstring>
#include <winsock2.h>

int main() {
    // 初始化Winsock库
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "Error initializing Winsock" << std::endl;
        return 1;
    }
    // 创建socket
    SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // 指定服务器地址和端口号
    std::string server_addr_str = "127.0.0.1";
    int port = 8000;
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_addr_str.c_str());
    server_addr.sin_port = htons(port);

    // 连接服务器
    if (connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    std::cout << "Connected to server " << server_addr_str << ":" << port << std::endl;

    // 发送数据到服务器
    std::string message = "Hello, server!";
    if (send(fd, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending data to server" << std::endl;
        return 1;
    }
    std::cout << "Sent message: " << message << std::endl;

    // 接收服务器的响应数据
    char buffer[1024] = { 0 };
    int num_bytes = recv(fd, buffer, sizeof(buffer), 0);
    if (num_bytes == SOCKET_ERROR) {
        std::cerr << "Error receiving data from server" << std::endl;
        return 1;
    }
    std::cout << "Received message: " << buffer << std::endl;

    // 关闭socket
    closesocket(fd);
    // 清理Winsock
    WSACleanup();
    return 0;
}