CXX = g++
CXXFlAGS = -O2 --std=c++17 -Wall -Wextra -g -I$(INCLUDE_DIR)
LDFlAGS =
INCLUDE_DIR = include
SRC_DIR = src
SRC_LIB = $(SRC_DIR)/networking.cc $(SRC_DIR)/protocol.cc $(SRC_DIR)/room.cc

all : client server

client : $(SRC_DIR)/client.cc $(SRC_LIB)
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

server : $(SRC_DIR)/server.cc $(SRC_LIB)
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

clean :
	$(RM) client server
