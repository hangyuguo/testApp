// getAvaliablePort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <WinSock2.h>


std::vector<int> get_available_ports(int start_port, int end_port);

int main() {
    int start_port = 8000;
    int end_port = 8100;
    std::vector<int> available_ports = get_available_ports(start_port, end_port);
    // 如果没有可用端口，报错退出
    if (available_ports.empty()) {
        std::cerr << "No available ports in range " << start_port << " - " << end_port << std::endl;
        return 1;
    }

    // 输出可用端口列表
    std::cout << "Available ports: ";
    for (int port : available_ports) {
        std::cout << port << " ";
    }
    std::cout << std::endl;

    // 在第一个可用端口上建立socket服务监听
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    int port = available_ports[0];
    sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket to port " << port << std::endl;
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error listening on port " << port << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << port << std::endl;

    // 等待客户端连接
    while (true) {
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }
        
        std::cout << "Client connected from " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // 处理客户端请求
        // ...

        closesocket(client_fd);
    }

    closesocket(server_fd);
}

std::vector<int> get_available_ports(int start_port, int end_port)
{
	std::vector<int> available_ports;
    // 初始化Winsock库
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw std::runtime_error("Error initializing Winsock");
    }

    // 创建socket
    SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == INVALID_SOCKET) {
        throw std::runtime_error("Error creating socket");
    }

    // 设置socket选项
    BOOL optval = TRUE;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
        throw std::runtime_error("Error setting socket options");
    }

    // 绑定socket到本地地址
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    for (int port = start_port; port <= end_port; port++) {
        addr.sin_port = htons(port);

        if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == 0) {
            // 端口可用，加入可用端口列表
            available_ports.push_back(port);

            // 关闭socket
            closesocket(fd);

            // 创建新socket，准备查找下一个可用端口
            fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (fd == INVALID_SOCKET) {
                throw std::runtime_error("Error creating socket");
            }

            // 设置socket选项
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
                throw std::runtime_error("Error setting socket options");
            }
        }
    }

    // 关闭socket
    closesocket(fd);

    // 清理Winsock库
    WSACleanup();

    return available_ports;
}