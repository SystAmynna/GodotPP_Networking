#ifndef GODOTPP_LINKING_CONTEXT_H
#define GODOTPP_LINKING_CONTEXT_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <functional>

using namespace godot;

using entity_factory = std::function<Node*>();

class linking_context : public Object{

    GDCLASS(linking_context, Object);

    // Maps NetworkTypeID (e.g., 1) to a Godot ClassName (e.g., "Player")
    HashMap<uint32_t, entity_factory> type_registry;

    // Maps Server-provided NetworkID to the actual Node instance
    HashMap<uint32_t, Node*> network_id_to_node;
    HashMap<Node*, uint32_t> node_to_network_id;

protected:
    static void _bind_methods();

public:

    // Registry Management
    void register_network_type(uint32_t type_id, entity_factory factory);

    // Entity Management
    void add_entity(uint32_t network_id, Node* node);
    void remove_entity(uint32_t network_id);

    Node* get_entity(uint32_t network_id) const;
    uint32_t get_network_id(Node* node) const;

    // Spawning logic (Called when server sends a spawn packet)
    Node* create_network_entity(uint32_t type_id, uint32_t network_id);

};


#endif //GODOTPP_LINKING_CONTEXT_H