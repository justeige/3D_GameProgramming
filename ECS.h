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
    //System_List               m_systems;
    std::map<ID, Bytes>       m_components;
    std::vector<Entity*>      m_entities;


    // entity methods
    Entity_Handle make_entity(Component_Base** components, ID const* components_ids, std::size_t num_components);
    void remove_entity(Entity_Handle* handle);

    template <class C1, class C2>
    Entity_Handle make_from(C1& c1, C2& c2)
    {
        Component_Base* components[] = { &c1, &c2 };
        u32 ids[] = { C1::Id, C2::Id };
        return make_entity(components, ids, 2);
    }


    // component methods
    template <class TComponent>
    void add_component(Entity_Handle entity, TComponent* c) { add_component_internal(handle_to_entity(entity), TComponent::Id, c); }
    void add_component_internal(Entity_Handle handle, std::vector<std::pair<u32, u32>>& entity, ID component_id, Component_Base* component);

    template <class TComponent>
    bool remove_component(Entity_Handle handle) { return remove_component_internal(handle, TComponent::Id); }
    bool remove_component_internal(Entity_Handle handle, ID component_id);
    void delete_component(ID component_id, u32 index);

    template <class TComponent>
    TComponent* component(Entity_Handle handle) { (TComponent*)component_internal(handle_to_entity(handle), m_components[TComponent::Id], TComponent::Id); }
    Component_Base* component_internal(std::vector<std::pair<u32, u32>>& entity_components, std::vector<u8>& arr, ID component_id);

    u32 find_least_common_component(std::vector<u32> const& types, std::vector<u32> const& flags);

    // system methods
    void update_systems(System_List& list, float delta);
    void update_system_with_multiple(u32 index, System_List systems, float delta, std::vector<u32> const& types, std::vector<Component_Base*>& component_param, std::vector<std::vector<u8>*>&);


    // accessors
    Entity* to_raw_type(Entity_Handle handle) const { return (Entity*)handle; }
    std::vector<std::pair<u32, u32>>& handle_to_entity(Entity_Handle handle) const { return to_raw_type(handle)->second; }
    u32 handle_to_entity_index(Entity_Handle handle) const { return to_raw_type(handle)->first; }
};
