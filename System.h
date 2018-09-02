#pragma once

// --------------------------------------------------
// systems are functors that just update components
// --------------------------------------------------

#include "Component.h"
#include <vector>

struct System_Base {

    System_Base(std::vector<ID> const& types_) : types(types_) {}

    virtual void update(float delta, Component_Base** m_components) {}

    std::vector<ID> types;
};