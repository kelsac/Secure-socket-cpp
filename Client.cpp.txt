#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <cstring>

// Funksjon som lytter etter meldinger fra serveren
void receive_messages(int socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            std::cout << "Mistet kontakt med serveren" << std::endl;
            exit(0);
        }
        std::cout << "Melding: " << buffer << std::endl;
    }
}

int main() {
    // 1. Lag socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        std::cout << "Feil: klarte ikke opprette socket" << std::endl;
        return 1;
    }

    // 2. Koble til server (Localhost: 127.0.0.1)
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Tilkobling feilet! (sjekk om serveren kjører)" << std::endl;
        return 1;
    }

    std::cout << "Koblet til! skriv melding her: " << std::endl;

    // 3. Start tråd for å lytte
    std::thread receiver(receive_messages, sock);
    receiver.detach();

    // 4. Send meldinger (Hovedtråden)
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(sock, message.c_str(), message.size(), 0);
    }

    close(sock);
    return 0;
}
