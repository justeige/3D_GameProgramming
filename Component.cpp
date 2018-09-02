#include "Component.h"

ID Component_Base::register_component(Component_Create_Function create_func, Component_Delete_Function delete_func, std::size_t size)
{
    ID id = types.size();
    types.push_back({ create_func, delete_func, size });
    return id;
}