#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <cstring>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <random>
#include <algorithm>
#include "../lib/lobby.h"

using namespace std;
#define OK_MESSAGE "OK"

string hashPassword(const string &password);
string createDefaultTables(sqlite3 *db);
string insertUser(sqlite3 *db, const string &username, const string &email, const string &password);
string insertGame(sqlite3 *db, const string &type, const string &password);
vector<Lobby> getLobbies(sqlite3 *db);

#endif