CXX = g++
CXXFlAGS = -O2 -Wall -Wextra -g -I$(INCLUDE_DIR)
LDFlAGS =
INCLUDE_DIR = include
SRC_DIR = src

all : client server

client : $(SRC_DIR)/client.cc $(SRC_DIR)/networking.cc
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

server : $(SRC_DIR)/server.cc $(SRC_DIR)/networking.cc
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

clean :
	$(RM) client server
