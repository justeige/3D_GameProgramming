#pragma once

// --------------------------------------------------
// systems are functors that just update components
// --------------------------------------------------

#include "Component.h"
#include <vector>

struct System_Base {

    enum Flags : u32 {
        FL_Basic,
        FL_Optional = 1
    };

    System_Base(std::vector<ID> const& types_) : types(types_) {}

    virtual void update(float delta, Component_Base** m_components) {}

    void add_component_type(u32 component_type, u32 component_flag = FL_Basic)
    {
        types.push_back(component_type);
        flags.push_back(component_flag);
    }

    // a system is valid if at least one component is NOT optional,
    // a system of only optional components is not supported
    bool is_valid() const
    {
        for_size(flags) {
            if ((flags[n] & FL_Optional) == 0) {
                return true;
            }
        }
        return false;
    }

    std::vector<ID>  types;
    std::vector<u32> flags;
};

struct System_List {

    std::vector<System_Base*> m_systems;

    bool add_system(System_Base& system);
    bool remove_system(System_Base& system);
    std::size_t size() const { return m_systems.size(); }
    System_Base* operator[](u32 index) { return m_systems[index]; }
};