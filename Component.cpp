#include "Component.h"

// static init for the concrete type - the other static inits can be in the header becaus they refer to templates!
std::vector<Component_Base::Allocation_Data>* Component_Base::types = nullptr;

ID Component_Base::register_component(Component_Create_Function create_func, Component_Delete_Function delete_func, std::size_t size)
{
    // lazy init
    if (types == nullptr) {
        types = new std::vector<Component_Base::Allocation_Data>();
    }

    ID id = types->size();
    types->push_back({ create_func, delete_func, size });
    return id;
}