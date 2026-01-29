#include <iostream>
#include <sys/socket.h> // Nettverk for Linux
#include <netinet/in.h>
#include <unistd.h>
#include <thread>       // For std::thread
#include <vector>       // For oversikt over clients
#include <mutex>        // For thread safety
#include <algorithm>    // For std::remove
#include <cstring>      // For memset

// Globale variabler som trådene deler 
std::vector<int> clients; // liste for allt sockets som er koblet på 
std::mutex clients_mutex; // Mutex lås for å hindre at flere tråder endrer listen samtidig 

// Funksjonen for å kjøre i egen tråd for hver klient 
void handle_client(int client_socket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer)); // Nullstille minne/ Clear buffer

        // 1. Mottar melding 
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        // sjekke om brukeren har logget av eller mistet nettet
        if (bytes_received <= 0) {
            std::cout << "En klient koblet fra." << std::endl;
            close(client_socket);

            // Fjerner brukeren trygt fra listen 
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            break;
        }

        std::cout << "Melding mottatt: " << buffer << std::endl;

        // 2. Sender meldingen videre til alle andre (broadcast)
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int sock : clients) {
            if (sock != client_socket) {
                send(sock, buffer, bytes_received, 0);
            }
        }
    }
}

int main() {
    // 1. Oppretter socket (IPv4 og TCP-Protokoll)
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket mislykket, prøv på nytt?");
        return 1;
    }

    // 2. Kobler socketen til en port så vi kan bli funnet
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Kjører på port 8080
    server_addr.sin_addr.s_addr = INADDR_ANY; // Lytte på alle tilgjengelige nettverkskort

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind Mislykket, prøv på nytt");
        return 1;
    }

    // 3. Sette serveren i lytte-modus!
    if (listen(server_socket, 5) < 0) {
        perror("Lytting mislykket, prøv på nytt");
        return 1;
    }

    std::cout << "Start Kelvins Chat Server..." << std::endl;

    // 4. Infinite loop som akspeterer nye folk som kobler seg på.
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            perror("Feil ved tilkobling (accept)");
            continue;
        }

        std::cout << "En ny bruker har koblet seg til!" << std::endl;

        // Legg til brukeren i listen (mutex for sikkerhet)
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        // Oppretter ny tråd som kun håndterer denne brukeren!
        std::thread t(handle_client, client_socket);
        t.detach(); // Lar tråden kjøre selvstendig i bakgrunnen
    }

    close(server_socket);
    return 0;
}
