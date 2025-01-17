#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

using namespace std;

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024];
    std::string message;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    string command;
    cout << "~ ";
    cin >> command;
    while (strcmp(command.c_str(), "exit") != 0) {
        message = command;
        send(client_socket, message.c_str(), message.size(), 0);
        std::cout << "Message sent\n";

        // Read the server's response
        int valread = read(client_socket, buffer, 1024);
        std::cout << "Server: " << buffer << std::endl;
        cout << "~ ";
        cin >> command;
    }

    // Close the socket
    close(client_socket);
}