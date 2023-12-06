#include "../lib/database.h"

using namespace std;

vector<string> split(const string &str, const char &delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

string generateRandomString()
{
    string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    random_device rd;
    mt19937 generator(rd());
    shuffle(str.begin(), str.end(), generator);
    return str.substr(0, 32);
}

string createSessionToken(const string &username)
{

    unsigned char hashed[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    string token = username + to_string(time(nullptr)) + generateRandomString();
    SHA256_Update(&sha256, token.c_str(), token.length());
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

pair<string, string> readConfig()
{
    string ip, port;
    ifstream file("config.txt");
    if (file.is_open())
    {
        getline(file, ip);
        getline(file, port);
        file.close();
    }
    return make_pair(ip, port);
}