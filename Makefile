CXX = g++
CXXFlAGS = -O3 --std=c++20 -Wall -Wextra -pedantic -g -I$(INCLUDE_DIR) -flto
LDFlAGS = -flto
INCLUDE_DIR = include
SRC_DIR = src

COMMON_DIR = $(SRC_DIR)/common
SERVER_DIR = $(SRC_DIR)/server
CLIENT_DIR = $(SRC_DIR)/client
COMMON_FILES = $(wildcard $(COMMON_DIR)/*.cc)
SERVER_FILES = $(wildcard $(SERVER_DIR)/*.cc)
CLIENT_FILES = $(wildcard $(CLIENT_DIR)/*.cc)
COMMON_OBJS = $(patsubst %.cc,%.o,$(COMMON_FILES))
SERVER_OBJS = $(patsubst %.cc,%.o,$(SERVER_FILES))
CLIENT_OBJS = $(patsubst %.cc,%.o,$(CLIENT_FILES))

all : client server

%.o : %.cc
	$(CXX) $(CXXFlAGS) -c -o $@ $^ $(LDFlAGS)

client : $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

server : $(SERVER_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFlAGS) -o $@ $^ $(LDFlAGS)

clean :
	$(RM) client server $(SERVER_OBJS) $(CLIENT_OBJS) $(COMMON_OBJS)
