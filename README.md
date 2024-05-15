# Simple Chat Room
A simple chat room in C++

The idea is roughly to have a server hosting multiple chat rooms which require passwords.

## Building
Run the command `make` to build both the server and the client or build them seperately using `make server` or `make client`.
`make clean` is used in order to clean all built files.

## Running
### Server
`./server port`

### Client
`./client server_ip server_port`

## Protocol
The communication between server and client follows the protocol described below.
TODO: descrive protocol, insert figure.

## Client usage
Since communication between client and servers follows a protocol, different clients can be implemented. The only client provided in this repository is a command-line interface where commands are prefixed by a forward slash `/`, and all other text is sent as a message to the server.

### Commands
|Command|Description|
|-------|-----------|
|`/join room_name`|Join the chat room specified by room_name|
|`/quit`|Shut down the client|