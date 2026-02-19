#include "ecs_declaration.h"
#include <iostream>

uint32_t global_net_id_counter = 0;

entt::entity handle_new_connection(entt::registry& registry, const std::string& client_ip) {
    // Create a new entity for the incoming connection
    const entt::entity entity = registry.create();

    // Assign a unique NetworkID to the entity
    NetworkID net_id{global_net_id_counter++};
    registry.emplace<NetworkID>(entity, net_id);

    // For demonstration, we can also add a ClientConnection component
    ClientConnection connection{client_ip};
    registry.emplace<ClientConnection>(entity, connection);

    return entity;
}

void broadcast_spawn(entt::registry& registry, entt::entity new_entity, GameSocket* socket_ptr) {
    // Get the NetworkID of the new entity
    const auto&[id] = registry.get<NetworkID>(new_entity);

    // Build packet
    SpawnPacket packet;
    packet.net_id = id;

    // Broadcast to all clients
    const auto view = registry.view<ClientConnection>();
    view.each([&packet, socket_ptr](auto &conn) {
        if (const int32_t send_result = net_socket_send(socket_ptr, conn.ip_address.c_str(), reinterpret_cast<const uint8_t*>(&packet), sizeof(packet)); send_result < 0) {
            std::cerr << "Failed to send spawn packet to " << conn.ip_address << std::endl;
        }

    });
}