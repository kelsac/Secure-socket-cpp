#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cstring>
#include "server.h" // inkludere headeren!

// Globale variabler
std::vector<int> clients;
std::mutex clients_mutex;

// Sender melding til alle andre
void broadcast_message(const char* message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int sock : clients) {
        if (sock != sender_socket) {
            send(sock, message, strlen(message), 0);
        }
    }
}

// Håndterer hver enkelt klient
void handle_client(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "En klient koblet fra." << std::endl;
            close(client_socket);
            
            // Fjern fra listen
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            break;
        }

        std::cout << "Melding mottatt: " << buffer << std::endl;
        broadcast_message(buffer, client_socket);
    }
}

// Selve hovedmotoren som starter alt
void start_server(int port) {
    // 1. Opprett Socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket feilet");
        exit(1);
    }

    // 2. Bind til port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind feilet");
        exit(1);
    }

    // 3. Listen
    if (listen(server_socket, 5) < 0) {
        perror("Listen feilet");
        exit(1);
    }

    std::cout << "Starter opp Kelvins Chat Server" << std::endl;

    // 4. Accept loop
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            perror("Accept feilet");
            continue;
        }

        std::cout << "En ny bruker logget på!" << std::endl;

        // Legg til i listen
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        // Start tråd
        std::thread t(handle_client, client_socket);
        t.detach();
    }
    
    close(server_socket);
}
