#include "ECS.h"
#include <algorithm>

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

Entity_Handle ECS::make_entity(Component_Base** entity_components, ID const* component_ids, std::size_t num_components)
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

        add_component_internal(handle, new_entity->second, id, entity_components[n]);
    }

    new_entity->first = m_entities.size();
    m_entities.push_back(new_entity);
    return handle;
}

void ECS::remove_entity(Entity_Handle* handle)
{
    auto& entity = handle_to_entity(handle);

    // first remove all components from the obj
    for (auto e : entity) {
        delete_component(e.first, e.second);
    }

    u32 dest_index = handle_to_entity_index(handle);
    u32 src_index  = m_entities.size() - 1;
    delete m_entities[dest_index];

    // swap the obj to delte to the back and delete from there
    m_entities[dest_index] = m_entities[src_index];
    m_entities[dest_index]->first = dest_index;
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
    for_size (n, entity_components) { /// TODO structed return [id, index] ?
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
    for_size(n, src_components) {
        if (component_id == src_components[n].first && src_index == src_components[n].second) {
            src_components[n].second = index;
            break; // this works only if there is just one component of a type per entity... create multi-components if more needed?
        }
    }
    arr.resize(src_index);
}

Component_Base* ECS::component_internal(std::vector<std::pair<u32, u32>>& entity_components, std::vector<u8>& arr, ID component_id)
{
    for_size(n, entity_components) {
        if (component_id == entity_components[n].first) {
            return (Component_Base*)&arr[entity_components[n].second];
        }
    }

    // component not found
    return nullptr;
}

void ECS::update_systems(System_List& systems, float delta)
{
    std::vector<Component_Base*>  component_param;
    std::vector<std::vector<u8>*> component_arrays;

    for_size(n, systems) {
        const auto types = systems[n]->types;
        if (types.size() == 1) {
            auto type_size = Component_Base::size_of(types[0]);
            auto arr = m_components[types[0]];
            for (u32 i = 0; i < arr.size(); i += type_size) {
                auto* component = (Component_Base*)&arr[i];
                systems[n]->update(delta, &component);
            }
        }
        else {
            update_system_with_multiple(n, systems, delta, types, component_param, component_arrays);
        }
    }
}

void ECS::update_system_with_multiple(u32 index, System_List systems, float delta, std::vector<u32> const & types, std::vector<Component_Base*>& component_param, std::vector<std::vector<u8>*>& component_arrays)
{
    auto component_flags = systems[index]->flags;

    component_param.resize(std::max(component_param.size(),   types.size()));
    component_arrays.resize(std::max(component_arrays.size(), types.size()));

    for_size(n, types) { component_arrays[n] = &m_components[types[n]]; }

    u32 min_size_index = find_least_common_component(types, component_flags);
    auto type_size = Component_Base::size_of(types[min_size_index]);
    auto& arr = *component_arrays[min_size_index];

    for (u32 i = 0; i < arr.size(); i += type_size) {
        component_param[min_size_index] = (Component_Base*)&arr[i];
        auto& entity_components = handle_to_entity(component_param[min_size_index]->entity);

        bool is_valid = true;
        for_size(n, types) {
            if (n == min_size_index) { continue; }

            component_param[n] = component_internal(entity_components, *component_arrays[n], types[n]);
            if (component_param[n] == nullptr && (component_flags[n] & System::FL_Optional) == 0) {
                is_valid = false;
                break;
            }
        }

        if (is_valid) {
            systems[index]->update(delta, &component_param[0]);
        }
    }
}

u32 ECS::find_least_common_component(std::vector<u32> const & types, std::vector<u32> const& flags)
{
    u32 min_size  = (u32)-1;
    u32 min_index = (u32)-1;

    for_size(n, types) {

        if ((flags[n] & System::FL_Optional) != 0) {
            continue;
        }

        auto type_size = Component_Base::size_of(types[n]);
        u32 size = m_components[types[n]].size() / type_size;
        if (size <= min_size) {
            min_size = size;
            min_index = n;
        }
    }

    return min_size;
}