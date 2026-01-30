#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <mutex>
#include <netinet/in.h>

// Deklarasjon av funksjoner 
void handle_client(int client_socket);
void start_server(int port);
void broadcast_message(const char* message, int sender_socket);

#endif 
