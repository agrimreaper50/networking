#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

struct Client {
    int socket;
    std::string name;
};

std::vector<Client> clients;
std::mutex clients_mutex;

void broadcast(const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto& client : clients) {
        if (client.socket != sender_socket) {
            send(client.socket, message.c_str(), message.size(), 0);
        }
    }
}

void handle_client(int client_socket) {
    char buffer[1024];
    
    // Ask for username
    std::string askName = "Enter your username: ";
    send(client_socket, askName.c_str(), askName.size(), 0);
    
    ssize_t bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(client_socket);
        return;
    }
    buffer[bytes] = '\0';
    std::string username(buffer);
    std::cout << username;
    username.erase(std::remove(username.begin(), username.end(), '\n'), username.end());

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back({client_socket, username});
    }

    std::string joinMsg = username + " joined the chat.\n";
    broadcast(joinMsg, client_socket);
    std::cout << joinMsg;

    // Main message loop
    while (true) {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::string msg = username + ": " + buffer;
        std::cout << msg;
        broadcast(msg, client_socket);
    }

    // Remove client on disconnect
    close(client_socket);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove_if(clients.begin(), clients.end(),
            [&](const Client& c){ return c.socket == client_socket; }), clients.end());
    }

    std::string leaveMsg = username + " left the chat.\n";
    broadcast(leaveMsg, -1);
    std::cout << leaveMsg;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);
    std::cout << "Server started on port " << PORT << std::endl;

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        std::thread(handle_client, client_socket).detach();
    }

    close(server_fd);
}
