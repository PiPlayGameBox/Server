flags = -Wall -Wextra -lsqlite3 -lssl -lcrypto -lpthread
libs = src/database.cpp src/utils.cpp 

run:
	g++ src/main.cpp $(libs) -o main $(flags)
	./main

cl:
	g++ src/client.cpp $(libs) -o client $(flags)
	./client

clean:
	rm -rf main client