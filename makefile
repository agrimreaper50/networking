CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# Executable names
SERVER = server
CLIENT = client

# Source files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp

# Default target: build both
all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC)

$(CLIENT): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Clean up compiled files
clean:
	rm -f $(SERVER) $(CLIENT)
