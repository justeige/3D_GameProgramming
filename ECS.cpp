#include "ECS.h"

ECS::~ECS()
{
    for (auto& component : m_components) {
        const auto type_size = Component_Base::size_of(component.first);
        auto free_func       = Component_Base::delete_function_of(component.first);

        for (u32 n = 0; n < component.second.size(); n += type_size) {
            free_func((Component_Base*)& component.second[n]);
        }
    }

    // free the entities
    for (u32 n = 0; n < m_entities.size(); ++n) { delete m_entities[n]; }
}

Entity_Handle ECS::make_entity(Component_Base* entity_components, ID const* component_ids, std::size_t num_components)
{
    auto new_entity = new Entity();
    Entity_Handle handle = (Entity_Handle)new_entity;

    for (u32 n = 0; n < num_components; ++n) {

        const auto id = component_ids[n];

        if (!Component_Base::is_valid(id)) {
            /// TODO logging
            delete new_entity;
            return nullptr;
        }

        auto create_func = Component_Base::create_function_of(id);

        std::pair<u32, u32> new_pair;
        new_pair.first  = id;
        new_pair.second = create_func(m_components[id], handle, &entity_components[n]);
        new_entity->second.push_back(new_pair);
    }

    new_entity->first = m_entities.size();
    m_entities.push_back(new_entity);
    return handle;
}

void ECS::remove_entity(Entity_Handle* handle)
{
    auto& entity = handle_to_entity(handle);

    // first remove all components from the obj
    for (u32 n = 0; n < entity.size(); ++n) {
        remove_component_internal(entity[n].first, entity[n].second);
    }

    u32 dest_index = handle_to_entity_index(handle);
    u32 src_index  = m_entities.size() - 1;
    delete m_entities[dest_index];

    // swap the obj to delte to the back and delete from there
    m_entities[dest_index] = m_entities[src_index];
    m_entities.pop_back();
}

void ECS::remove_component_internal(ID component_id, u32 index)
{
}
