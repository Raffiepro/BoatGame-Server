#include <SFML/Network.hpp>
#include <unordered_map>
#include <thread>
#include <iostream>

struct playerDat {
    sf::IpAddress ip;
    unsigned short port;
    float x,y,z,dirY,dirX;
};

std::unordered_map<unsigned char, playerDat> players;

void renderText() {
    while(true) {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        for(int y=-5;y<5;y++) {
            for(int x=-10;x<10;x++) {
                bool foundPlayer=false;
                for(const auto& i : players) {
                    if(x==(int)i.second.x && y==(int)i.second.z) {
                        foundPlayer=true;
                    }
                }
                if(foundPlayer) {
                    std::cout<<'#';
                } else {
                    std::cout<<' ';
                }
            }
            std::cout<<'\n';
        }
    }
}

int main() {
    sf::UdpSocket socket;
    socket.bind(54000);
    socket.setBlocking(false);

    std::cout << "Server started on port 54000.\n";

    sf::Packet packet;
    playerDat p;
    sf::IpAddress& senderIp = p.ip;
    unsigned short& senderPort = p.port;

    std::thread r(renderText);

    while (true) {
        while (socket.receive(packet, senderIp, senderPort) == sf::Socket::Done) {
            int playerId;
            packet >> playerId >> p.x >> p.y >> p.z >> p.dirY >> p.dirX;

            players[playerId] = p;
            //std::cout<<playerId<<' '<<x<<' '<<y<<' '<<z<<' '<<dirY<<' '<<dirX<<'\n';

            sf::Packet outPacket;
            outPacket << playerId << p.x << p.y << p.z << p.dirY << p.dirX;
            for(const auto& i : players) {
                if(i.first!=playerId) {
                    socket.send(outPacket, i.second.ip, i.second.port);
                    //std::cout<<"Sent packet to "<<i.second.ip<<':'<<i.second.port<<'\n';
                }
            }
            packet.clear();
        }
        //sf::sleep(sf::milliseconds(1)); // still reduce CPU usage
    }
    return 0;
}