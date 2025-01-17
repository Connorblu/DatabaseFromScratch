#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>
#include <unistd.h>
#include <sys/socket.h>
#include "server_data.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

using namespace std;

void handle_client(int client_socket, FILE *db) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the received string
        printf("Received: %s\n", buffer);

        // Process the command 
        stringstream ss(buffer);
        string word;
        while (ss >> word) {
            //Handle Create command
            if (word == "CREATE") {
                string key;
                string value;
                ss >> key;
                ss >> value;
                fprintf(db, "%s,%s\n", key.c_str(), value.c_str());
            }
            else if (word == "READ") {
                string key;
                ss >> key;
                char line[1024];
                fseek(db, 0, SEEK_SET);
                while (fgets(line, 1024, db) != NULL) {
                    char *k = strtok(line, ",");
                    char *v = strtok(NULL, ",");
                    if (strcmp(k, key.c_str()) == 0) {
                        send(client_socket, v, strlen(v), 0);
                        break;
                    }
                }
            }
        }
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    //Check if database file exists. If not, create a new one
    FILE *db = fopen("database.txt", "r+");
    if (db == NULL) {
        db = fopen("database.txt", "w+");
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept and handle clients
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) >= 0) {
        printf("Client connected\n");
        handle_client(client_socket, db);
    }

    close(server_socket);
    return 0;
}