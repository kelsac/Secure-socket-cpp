# Multithreaded TCP Chat Server (C++ / Linux)

Dette prosjektet er en implementasjon av en flertrådet chat-server og klient, skrevet fra bunnen av i C++ for Linux-miljøer. 

Formålet med prosjektet var å demonstrere lavnivå nettverksprogrammering, trådsikkerhet og minnehåndtering uten bruk av høynivå-rammeverk.

## Funksjonalitet
* **Multithreading:** Serveren håndterer flere klienter samtidig ved bruk av `std::thread`.
* **Trådsikkerhet:** Implementerer `std::mutex` for å hindre "Race Conditions" ved skriving til delte ressurser (brukerlisten).
* **Linux Sockets:** Bruker standard POSIX socket API (`<sys/socket.h>`) for robust nettverkskommunikasjon.
* **Broadcast:** Meldinger fra én bruker sendes umiddelbart til alle andre tilkoblede brukere.

## Teknisk Stack
* **Språk:** C++ (kompilert med g++)
* **OS:** Utviklet og testet på Kali Linux / Debian
* **Biblioteker:** `<sys/socket.h>`, `<thread>`, `<mutex>`, `<vector>`

## Hvordan kjøre prosjektet

### 1. Kompilering
Bruk g++ til å kompilere kildekoden. Husk `-pthread` flagget for trådstøtte.

```bash
# Kompiler server
g++ server.cpp -o server -pthread

# Kompiler klient
g++ client.cpp -o client -pthread