#include "../lib/database.h"
#include "../lib/utils.h"
#include "../lib/lobby.h"
#include <signal.h>

#define REQUEST_BUFFER_SIZE 1024

using namespace std;
mutex dbMutex;

vector<Lobby> lobbies;

void onInterrupt(int socket)
{
    close(socket);
    cout << "Server closed" << endl;
    exit(0);
}

bool checkUserLogin(sqlite3 *db, const string &username, const string &password)
{
    string query = "SELECT password FROM Users WHERE username = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing SELECT query: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    if (result == SQLITE_ROW)
    {
        string hashedPassword = string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
        sqlite3_finalize(stmt);

        return hashedPassword == hashPassword(password);
    }
    else if (result == SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return false;
    }
    else
    {
        cerr << "Error executing SELECT query: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

bool checkSessionToken(int clientSocket, const string &userToken, const string &sessionToken)
{
    if (userToken != sessionToken)
    {
        string response = "ERROR|YOU ARE NOT AUTHORIZED";
        send(clientSocket, response.c_str(), REQUEST_BUFFER_SIZE, 0);
        return false;
    }
    return true;
}

void handleClient(int clientSocket)
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("db/piplaybox.db", &db);

    if (rc)
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }
    else
    {
        cout << "Opened database successfully" << endl;
    }
    char buffer[1024];
    vector<string> params;
    int bytesRead;
    string sessionToken;
    string response;
    string username;

    // while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    // {
    //     buffer[bytesRead] = '\0';
    //     cout << "Received message from client: " << buffer << endl;

    //     // Echo back to the client
    //     send(clientSocket, buffer, bytesRead, 0);
    // }

    while (true)
    {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';
        cout << "Received message from client: " << buffer << endl;
        params = split(buffer, '|');
        if (params[0] == "REGISTER")
        {
            {
                lock_guard<mutex> lock(dbMutex);
                insertUser(db, params[1], params[2], hashPassword(params[3]));
            }
        }
        else if (params[0] == "LOGIN")
        {
            {
                lock_guard<mutex> lock(dbMutex);
                username = params[1];
                sessionToken = createSessionToken(username);
                if (checkUserLogin(db, params[1], params[2]))
                {
                    response = "OK|";
                    response += sessionToken;
                }
                else
                {
                    response = "ERROR";
                }
            }

            send(clientSocket, response.c_str(), REQUEST_BUFFER_SIZE, 0);
        }
        else if (params[0] == "GETLOBBIES")
        {
            checkSessionToken(clientSocket, params[1], sessionToken);
            response = "OK";
            for (size_t i = 0; i < lobbies.size(); i++)
            {
                response += '|' + to_string(lobbies[i].id) + '|' + lobbies[i].type;
                for (size_t j = 0; j < lobbies[i].players.size(); j++)
                {
                    response += '|' + lobbies[i].players[j];
                }
            }
            send(clientSocket, response.c_str(), REQUEST_BUFFER_SIZE, 0);
        }
        else if (params[0] == "CONNECT")
        {
            checkSessionToken(clientSocket, params[1], sessionToken);
            response = "BYE";
            send(clientSocket, response.c_str(), REQUEST_BUFFER_SIZE, 0);
            break;
        }
        else if (params[0] == "QUIT")
        {
            checkSessionToken(clientSocket, params[1], sessionToken);
            response = "BYE";
            send(clientSocket, response.c_str(), REQUEST_BUFFER_SIZE, 0);
            break;
        }
    }

    cout << "Client disconnected" << endl;
    close(clientSocket);
    sqlite3_close(db);
}

void createDatabase()
{
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("db/piplaybox.db", &db);

    if (rc)
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        exit(-1);
    }
    else
    {
        cout << "Opened database successfully" << endl;
    }

    createDefaultTables(db);
    insertUser(db, "admin", "admin@admin.com", "123456");
    insertGame(db, "LUDO","123456");

    sqlite3_close(db);
}

int createConnection()
{
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    pair<string, string> config = readConfig();
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(config.first.c_str());
    serverAddress.sin_port = htons(stoi(config.second));

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        cerr << "Error binding socket" << endl;
        perror("bind");
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 10) == -1)
    {
        cerr << "Error listening for connections" << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Server listening on port " << config.second << "..." << endl;

    return serverSocket;
}

int connectClient(int serverSocket)
{
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);

    if (clientSocket == -1)
    {
        return clientSocket;
    }

    cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << endl;

    return clientSocket;
}

int main()
{
    signal(SIGINT, onInterrupt);
    // createDatabase();
    sqlite3 *db;
    sqlite3_open("db/piplaybox.db", &db);
    lobbies = getLobbies(db);
    sqlite3_close(db);
    int serverSocket = createConnection();

    while (true)
    {
        int clientSocket = connectClient(serverSocket);
        if (clientSocket == -1)
        {
            cerr << "Error accepting connection" << endl;
            continue;
        }
        thread(handleClient, clientSocket).detach();
    }

    if (close(serverSocket) == -1)
    {
        cerr << "Error closing socket" << endl;
        return -1;
    }

    return 0;
}