#ifndef LOBBY_H
#define LOBBY_H

#include "database.h"

using namespace std;

class Lobby {
    public:
        int id;
        string type;
        vector<int> players;
};

#endif