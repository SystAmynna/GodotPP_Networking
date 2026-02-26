#include "../include/linking_context.h"



void linking_context::register_type(uint32_t type_id, entity_factory factory) {
    type_registry[type_id] = factory;
}

void linking_context::add_entity(uint32_t network_id, Node* node) {
    if (!node) return;
    network_id_to_node[network_id] = node;
    node_to_network_id[node] = network_id;
}

void linking_context::remove_entity(uint32_t network_id) {
    if (network_id_to_node.has(network_id)) {
        node_to_network_id.erase(network_id_to_node[network_id]);
        network_id_to_node.erase(network_id);
    }
}

Node* linking_context::get_entity(uint32_t network_id) const {
    auto it = network_id_to_node.find(network_id);
    return it ? *it : nullptr;
}

uint32_t linking_context::get_network_id(Node* node) const {
    auto it = node_to_network_id.find(node);
    return it ? *it : 0;
}

Node* linking_context::create_network_entity(uint32_t type_id, uint32_t network_id) {
    if (!type_registry.has(type_id)) {
        UtilityFunctions::printerr("Network Type ID ", type_id, " is not registered!");
        return nullptr;
    }

    // Execute the lambda to get a new instance
    Node* node = type_registry[type_id]();

    if (node) {
        add_entity(network_id, node);
    }

    return node;
}

void linking_context::_bind_methods() {
    // Note: We can't easily bind std::function to GDScript,
    // so this system is primarily for C++ internal use.
    ClassDB::bind_method(D_METHOD("get_entity", "network_id"), &linking_context::get_entity);
}