#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
#include <string>
#pragma comment(lib, "ws2_32.lib")
std::vector<SOCKET> clients; // Store connected clients
void handleClient(SOCKET clientSocket) {
char buffer[1024];
while (true) {
memset(buffer, 0, sizeof(buffer));
int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
if (bytesReceived <= 0) {
std::cout << "A client disconnected.\n";
closesocket(clientSocket);
break;
}
buffer[bytesReceived] = '\0';
std::cout << "Message from a client: " << buffer << "\n";
for (SOCKET sock : clients) {
if (sock != clientSocket) { // Don't send the message back to the sender
send(sock, buffer, bytesReceived, 0);
}
}
}
}
int main() {
WSADATA wsa;
SOCKET serverSocket, clientSocket;
sockaddr_in serverAddr, clientAddr;
int addrLen = sizeof(clientAddr);
std::cout << "Initializing Winsock...\n";
if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
std::cerr << "Failed to initialize Winsock. Error: " <<
WSAGetLastError() << "\n";
return -1;
}
serverSocket = socket(AF_INET, SOCK_STREAM, 0);
if (serverSocket == INVALID_SOCKET) {
std::cerr << "Socket creation failed. Error: " <<
WSAGetLastError() << "\n";
WSACleanup();
return -1;
}
serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = INADDR_ANY;
serverAddr.sin_port = htons(5000);
if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
std::cerr << "Bind failed. Error: " << WSAGetLastError() <<
"\n";
closesocket(serverSocket);
WSACleanup();
return -1;
}
std::cout << "Bind successful. Server ready on port 5000.\n";
if (listen(serverSocket, 5) == SOCKET_ERROR) {
std::cerr << "Listen failed. Error: " << WSAGetLastError() <<
"\n";
closesocket(serverSocket);
WSACleanup();
return -1;
}
std::cout << "Waiting for client connections...\n";
while (true) {
clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
if (clientSocket == INVALID_SOCKET) {
std::cerr << "Accept failed. Error: " << WSAGetLastError()
<< "\n";
continue;
}
std::cout << "New client connected.\n";
clients.push_back(clientSocket); // Add client to list
std::thread(handleClient, clientSocket).detach();
}
closesocket(serverSocket);
WSACleanup();
return 0;
}
