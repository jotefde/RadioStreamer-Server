#include "main.h"
#include "Server.h"

int main(int argc, char** argv) {


   /* if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    char* end;
    long port = strtol(argv[1], &end, 10);
    if (*end != 0 || port < 1 || port > 65535) {
        printf("Invalid argument: %s is not a port number\nUsage: %s <ip> <port>\n", argv[2], argv[0]);
        return 1;
    }*/
    long port = 1337;
    //std::thread serverListener(&Server::Listen, );
    std::thread serverLoop(&Server::Run, Server(port));
    Utils::Print("Server is listening on 0.0.0.0:" + to_string(port));
    //serverListener.join();
    serverLoop.join();

    return 0;
}
