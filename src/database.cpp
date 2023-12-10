#include "../lib/database.h"

using namespace std;

string hashPassword(const string &password)
{
    unsigned char hashed[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hashed, &sha256);

    BIO *bio, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, hashed, SHA256_DIGEST_LENGTH);
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bptr);

    string hashedStr(bptr->data, bptr->length - 1);
    BIO_free_all(bio);

    return hashedStr;
}

string createDefaultTables(sqlite3 *db)
{
    string usersQuery = "CREATE TABLE Users("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT NOT NULL UNIQUE,"
                   "email TEXT NOT NULL,"
                   "password TEXT NOT NULL);";

    string LobbiesQuery = "CREATE TABLE Lobbies("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "type TEXT NOT NULL,"
                   "password TEXT NOT NULL);";

    sqlite3_stmt *stmt;
    int result = sqlite3_prepare_v2(db, usersQuery.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK)
    {

        return sqlite3_errmsg(db);
    }

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db);
    }

    sqlite3_finalize(stmt);

    result = sqlite3_prepare_v2(db, LobbiesQuery.c_str(), -1, &stmt, nullptr);

    if (result != SQLITE_OK)
    {

        return sqlite3_errmsg(db);
    }

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db);
    }

    sqlite3_finalize(stmt);
    return OK_MESSAGE;
}

string insertGame(sqlite3 *db, const string &type, const string &password)
{
    string insertQuery = "INSERT INTO Lobbies (type, password) VALUES (?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return sqlite3_errmsg(db);
    }

    string hashed = hashPassword(password);

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db);
    }

    sqlite3_finalize(stmt);
    return OK_MESSAGE;
}

string insertUser(sqlite3 *db, const string &username, const string &email, const string &password)
{
    string insertQuery = "INSERT INTO Users (username, email, password) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, insertQuery.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return sqlite3_errmsg(db);
    }

    string hashed = hashPassword(password);

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashed.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db);
    }

    sqlite3_finalize(stmt);
    return OK_MESSAGE;
}

vector<Lobby> getLobbies(sqlite3 *db)
{
    string query = "SELECT id, type FROM Lobbies";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        cerr << "Error preparing SELECT query: " << sqlite3_errmsg(db) << endl;
        return vector<Lobby>();
    }

    vector<Lobby> lobbies;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Lobby lobby;
        lobby.id = sqlite3_column_int(stmt, 0);
        lobby.type = string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
        for (int i = 0; i < 4; i++)
        {
            lobby.players.push_back("empty");
        }
        lobbies.push_back(lobby);
    }

    sqlite3_finalize(stmt);
    return lobbies;
}
