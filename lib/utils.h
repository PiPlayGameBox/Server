#ifndef UTILS_H
#define UTILS_H

#include "database.h"

using namespace std;

vector<string> split(const string &str, const char &delimiter);
string generateRandomString();
string createSessionToken(const string &username);
pair<string, string> readConfig();

#endif