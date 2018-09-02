#pragma once
// --------------------------------------------------
// connector of the entity-component-system
// --------------------------------------------------

#include "Common.h"
#include "Component.h"
#include "System.h"

#include <vector>
#include <map>

using Entity = std::pair<ID, std::vector<std::pair<u32, u32>>>; /// TODO make readable with some typedefs or create new struct?

struct ECS final {

    ECS() {}
    ~ECS();

    // no copy & move
    ECS(ECS const& other) = delete;
    ECS(ECS && other)     = delete;
    void operator=(ECS const& other) = delete;
    void operator=(ECS && other)     = delete;

    // data
    std::vector<System_Base*> m_systems;
    std::map<ID, Bytes>       m_components;
    std::vector<Entity*>      m_entities;


    // entity methods
    Entity_Handle make_entity(Component_Base* components, ID const* components_ids, std::size_t num_components);
    void remove_entity(Entity_Handle* handle);

    // component methods
    template <class TComponent>
    void add_component(Entity_Handle handle, TComponent* c);

    template <class TComponent>
    void remove_component(Entity_Handle handle);

    void remove_component_internal(ID component_id, u32 index);

    template <class TComponent>
    void component(Entity_Handle handle);

    // system methods
    void add_system(System_Base& system) { m_systems.push_back(&system); }
    void update_systems(float delta);
    void remove_system(System_Base& system);

    // accessors
    Entity* to_raw_type(Entity_Handle handle) const { return (Entity*)handle; }
    std::vector<std::pair<u32, u32>>& handle_to_entity(Entity_Handle handle) const { return to_raw_type(handle)->second; }
    u32 handle_to_entity_index(Entity_Handle handle) const { return to_raw_type(handle)->first; }
};
