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

        add_component_internal(handle, new_entity->second, id, &entity_components[n]);
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
        delete_component(entity[n].first, entity[n].second);
    }

    u32 dest_index = handle_to_entity_index(handle);
    u32 src_index  = m_entities.size() - 1;
    delete m_entities[dest_index];

    // swap the obj to delte to the back and delete from there
    m_entities[dest_index] = m_entities[src_index];
    m_entities.pop_back();
}

void ECS::add_component_internal(Entity_Handle handle, std::vector<std::pair<u32, u32>>& entity, ID component_id, Component_Base * component)
{
    auto create_func = Component_Base::create_function_of(component_id);

    std::pair<u32, u32> new_pair;
    new_pair.first = component_id;
    new_pair.second = create_func(m_components[component_id], handle, component);
    entity.push_back(new_pair);
}

bool ECS::remove_component_internal(Entity_Handle handle, ID component_id)
{
    auto& entity_components = handle_to_entity(handle);
    for (u32 n = 0; n < entity_components.size(); ++n) {
        if (component_id == entity_components[n].first) {
            delete_component(entity_components[n].first, entity_components[n].second);
            u32 src_index = entity_components.size() - 1;
            u32 dest_index = n;

            // swap and delete the end
            entity_components[dest_index] = entity_components[src_index];
            entity_components.pop_back();
            return true;
        }
    }

    return false;
}

void ECS::delete_component(ID component_id, u32 index)
{
    auto& arr = m_components[component_id];
    auto delete_func = Component_Base::delete_function_of(component_id);
    auto type_size = Component_Base::size_of(component_id);
    u32 src_index = arr.size() - type_size;

    auto dest_component = (Component_Base*)&arr[index];
    auto src_component = (Component_Base*)&arr[src_index];
    delete_func(dest_component);

    // special case: last element
    if (index == src_index) {
        arr.resize(src_index);
        return;
    }

    ::memcpy(dest_component, src_component, type_size);
    auto& src_components = handle_to_entity(src_component->entity);
    for (u32 n = 0; n < src_components.size(); ++n) {
        if (component_id == src_components[n].first && src_index == src_components[n].second) {
            src_components[n].second = index;
            break; // this works only if there is just one component of a type per entity... create multi-components if more needed?
        }
    }
    arr.resize(src_index);
}

Component_Base* ECS::component_internal(std::vector<std::pair<u32, u32>>& entity_components, ID component_id)
{
    for (u32 n = 0; n < entity_components.size(); ++n) {
        if (component_id == entity_components[n].first) {
            return (Component_Base*)&m_components[component_id][entity_components[n].second];
        }
    }

    // component not found
    return nullptr;
}
