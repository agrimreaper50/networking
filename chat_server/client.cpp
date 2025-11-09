#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>

void receiveMessages(int sock) {
    char buffer[1024];
    while (true) {
        ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::cout << buffer;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed.\n";
        return 1;
    }

    std::thread(receiveMessages, sock).detach();

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        send(sock, msg.c_str(), msg.size(), 0);
    }

    close(sock);
    return 0;
}
