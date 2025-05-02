#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>

struct playerDat {
    sf::IpAddress ip;
    unsigned short port;
};

std::unordered_map<unsigned char, playerDat> players;

int main() {
    sf::UdpSocket socket;
    socket.bind(54000);
    socket.setBlocking(false);

    std::cout << "Server started on port 54000.\n";


    sf::Packet packet;
    playerDat p;
    sf::IpAddress& senderIp = p.ip;
    unsigned short& senderPort = p.port;

    while (true) {
        while (socket.receive(packet, senderIp, senderPort) == sf::Socket::Done) {
            int playerId;
            float x, y, z, dirY, dirX;
            packet >> playerId >> x >> y >> z >> dirY >> dirX;

            players[playerId] = p;
            //std::cout<<playerId<<' '<<x<<' '<<y<<' '<<z<<' '<<dirY<<' '<<dirX<<'\n';

            sf::Packet outPacket;
            outPacket << playerId << x << y << z << dirY << dirX;
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
