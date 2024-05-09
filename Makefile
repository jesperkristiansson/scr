CXX = g++
CXXFlAGS = -O2 -Wall -Wextra -g -I$(INCLUDE_DIR)
LDFlAGS =
INCLUDE_DIR = include
SRC_DIR = src

all : client server

client : $(SRC_DIR)/client.cc
	$(CXX) $(CXXFlAGS) -o $@ $< $(LDFlAGS)

server : $(SRC_DIR)/server.cc
	$(CXX) $(CXXFlAGS) -o $@ $< $(LDFlAGS)

clean :
	$(RM) client server
