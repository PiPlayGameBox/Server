#include "../lib/database.h"
#include "../lib/utils.h"

#define REQUEST_BUFFER_SIZE 1024

using namespace std;

void sendRegisterRequest(const int socket, const string &username, const string &email, const string &password)
{
    string request = "REGISTER|" + username + "|" + email + "|" + password;
    send(socket, request.c_str(), REQUEST_BUFFER_SIZE, 0);

    // Receive the response from the server
    char buffer[REQUEST_BUFFER_SIZE];
    int bytesRead = recv(socket, buffer, REQUEST_BUFFER_SIZE, 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        std::cout << "Received message from server: " << buffer << std::endl;
    }
}

string sendLoginRequest(const int socket, const string &username, const string &password)
{
    string request = "LOGIN|" + username + "|" + password;
    send(socket, request.c_str(), REQUEST_BUFFER_SIZE, 0);

    // Receive the response from the server
    char buffer[REQUEST_BUFFER_SIZE];
    int bytesRead = recv(socket, buffer, REQUEST_BUFFER_SIZE, 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        std::cout << "Received message from server: " << buffer << std::endl;
    }

    return string(buffer);
}

string sendGetLobbiesRequest(const int socket, const string &sessionToken)
{
    string request = "GETLOBBIES|" + sessionToken;
    send(socket, request.c_str(), REQUEST_BUFFER_SIZE, 0);

    // Receive the response from the server
    char buffer[REQUEST_BUFFER_SIZE];
    int bytesRead = recv(socket, buffer, REQUEST_BUFFER_SIZE, 0);
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
        std::cout << "Received message from server: " << buffer << std::endl;
    }

    return string(buffer);
}

int connectToServer()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    pair<string, string> config = readConfig();
    sockaddr_in serverAddress;
    string sessionToken;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(config.first.c_str());
    serverAddress.sin_port = htons(stoi(config.second));

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return -1;
    }

    return clientSocket;
}

int main()
{
    int clientSocket = connectToServer();
    // string sessionToken = sendLoginRequest(clientSocket, "admin", "123456");
    string lobbies = sendGetLobbiesRequest(clientSocket, "123456");
    cout << lobbies << endl;

    close(clientSocket);

    return 0;
}