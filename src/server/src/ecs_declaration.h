#pragma once

#include <string>
#include <entt/entt.hpp>
#include <snl.h>

struct NetworkID {
    uint32_t id;
};

struct ClientConnection {
    std::string ip_address;
};

struct SpawnPacket {
    uint32_t packet_type = 1; // Arbitrary value
    uint32_t net_id{};
};

entt::entity handle_new_connection(entt::registry& registry, const std::string& client_ip);
void broadcast_spawn(entt::registry& registry, entt::entity new_entity, GameSocket* socket_ptr);