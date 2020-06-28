#ifndef NO_LINK

#include <array>

#include "GBASockClient.h"

// Currently only for Joybus communications

GBASockClient::GBASockClient(sf::IpAddress _server_addr)
{
    if (_server_addr == sf::IpAddress::None)
        server_addr = sf::IpAddress::getPublicAddress();
    else
        server_addr = _server_addr;

    client.connect(server_addr, 0xd6ba);
    client.setBlocking(false);

    ReceivePackets(true);

    is_disconnected = false;
}

GBASockClient::~GBASockClient()
{
    Disconnect();
}

void GBASockClient::ReceivePackets(bool block) {
    if (block) {
        sf::SocketSelector Selector;
        Selector.add(client);
        Selector.wait();
    }

    sf::Packet packet;
    auto res = client.receive(packet);
    if (res != sf::Socket::Done && (block || res != sf::Socket::NotReady)) {
        Disconnect();
        return;
    }

    if (IsDisconnected()) {
        packet >> is_movie;
    }
    while (!packet.endOfPacket()) {
        GBAPacket cmd;
        packet >> cmd.data[0] >> cmd.data[1] >> cmd.data[2] >> cmd.data[3] >> cmd.data[4] >> cmd.time >> cmd.pad;
        cmds.emplace(std::move(cmd));
    }
}

void GBASockClient::Send(std::vector<uint8_t> data, uint64_t recvd_time)
{
    if (IsMovie())
        return;

    if (data.size() > std::numeric_limits<uint8_t>::max())
        return;

    sf::Packet packet;
    packet << static_cast<uint8_t>(data.size());
    for (auto b : data)
        packet << b;
    packet << recvd_time;

    client.send(packet);
}

bool GBASockClient::ReceiveCmd(GBAPacket& cmd, bool block)
{
    if (IsDisconnected())
        return false;

    ReceivePackets(block && cmds.empty());
    if (IsDisconnected())
        return false;

    if (cmds.empty())
        return false;

    cmd = cmds.front();
    cmds.pop();

    return true;
}

void GBASockClient::Disconnect()
{
    is_disconnected = true;
    client.disconnect();
}
#endif // NO_LINK
