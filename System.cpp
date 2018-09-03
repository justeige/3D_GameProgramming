#include "System.h"

bool System_List::add_system(System_Base& system)
{
    if (!system.is_valid()) {
        return false;
    }
    m_systems.push_back(&system);
    return true;
}

bool System_List::remove_system(System_Base& system)
{
    for_size(n, m_systems) {
        if (m_systems[n] == &system) {
            m_systems.erase(std::begin(m_systems) + n);
            return true;
        }
    }

    // system not found
    return false;
}