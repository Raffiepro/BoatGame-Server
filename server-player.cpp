#include <SFML/Network.hpp>
#include <iostream>

sf::IpAddress player1Ip;
unsigned short player1Port = 0;
bool player1Connected = false;

sf::IpAddress player2Ip;
unsigned short player2Port = 0;
bool player2Connected = false;

int main() {
    sf::UdpSocket socket;
    socket.bind(54000);
    socket.setBlocking(false);

    std::cout << "Server started on port 54000.\n";

    while (true) {
        sf::Packet packet;
        sf::IpAddress senderIp;
        unsigned short senderPort;

        if (socket.receive(packet, senderIp, senderPort) == sf::Socket::Done) {
            int playerId;
            float x, y, z, dirY;
            packet >> playerId >> x >> y >> z >> dirY;
            //std::cout<<playerId<<' '<<x<<' '<<y<<' '<<z<<' '<<dirY<<'\n';

            if (playerId == 1) {
                player1Ip = senderIp;
                player1Port = senderPort;
                player1Connected = true;

                if (player2Connected) {
                    sf::Packet outPacket;
                    outPacket << playerId << x << y << z << dirY;
                    socket.send(outPacket, player2Ip, player2Port);
                }

            } else if (playerId == 2) {
                player2Ip = senderIp;
                player2Port = senderPort;
                player2Connected = true;

                if (player1Connected) {
                    sf::Packet outPacket;
                    outPacket << playerId << x << y << z << dirY;
                    socket.send(outPacket, player1Ip, player1Port);
                }
            }
        }

        sf::sleep(sf::milliseconds(1));
    }

    return 0;
}