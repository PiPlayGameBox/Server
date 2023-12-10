#ifndef LOBBY_H
#define LOBBY_H

#include "database.h"

using namespace std;

class Lobby {
    public:
        int id;
        string type;
        vector<int> players;

        void print()
        {
            cout << "Lobby ID: " << id << endl;
            cout << "Lobby Type: " << type << endl;
            cout << "Lobby Players: ";
            for (size_t i = 0; i < players.size(); i++)
            {
                cout << players[i] << " ";
            }
            cout << endl;
        }
};

#endif