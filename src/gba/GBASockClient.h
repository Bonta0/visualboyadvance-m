#pragma once

#include <array>
#include <queue>
#include <vector>

#include <SFML/Network.hpp>

#include "../common/Types.h"

struct GBAPacket {
    std::array<uint8_t, 5> data;
    uint64_t time;
    uint16_t pad;
};

class GBASockClient {
public:
    GBASockClient(sf::IpAddress _server_addr);
    ~GBASockClient();

    void Send(std::vector<uint8_t> data, uint64_t recvd_time);
    bool ReceiveCmd(GBAPacket& cmd, bool block);

    void Disconnect();

    bool IsDisconnected() {
        return is_disconnected;
    }

    bool IsMovie() {
        return is_movie;
    }

private:
    void ReceivePackets(bool block);

    sf::IpAddress server_addr;
    sf::TcpSocket client;

    std::queue<GBAPacket> cmds;

    bool is_disconnected = true;
    bool is_movie = false;
};
