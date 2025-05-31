#include "mininet.hpp"
#include <vector>
#include <iostream>

struct playerDat {
    unsigned short port;
    float x,y,z,dirY,dirX;
};

int sockaddr_in_equal(const struct sockaddr_in *a, const struct sockaddr_in *b) {
    return (a->sin_family == b->sin_family) &&
           (a->sin_port == b->sin_port) &&
           (a->sin_addr.s_addr == b->sin_addr.s_addr);
}

std::vector<sockaddr_in> players;

int main() {
    UDPServer s(54000);
    s.setBlocking(false);

    std::cout << "Mewo meow meow mowe meow mow mew meeow on port 54000.\n";

    char buff[21];
    memset(buff, 0, 21);

    while (true) {
        sockaddr_in client;
        ssize_t size = s.recv(&client, buff, 21);
        
        if (size > 0) {
            bool found = false;
            for(auto& i : players) {
                if(!sockaddr_in_equal(&client, &i)) {
                    s.send(&i, buff, size);
                }
                else
                    found = true;
            }
            if(!found)  players.push_back(client);
        }
    }
    return 0;
}