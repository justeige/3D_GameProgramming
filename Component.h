#pragma once
// --------------------------------------------------
// base for the entity-component-system
// --------------------------------------------------

#include <vector>
#include "Common.h"

// forward decl.
struct Component_Base;


// typedefs
using Entity_Handle = void*; /// TODO use std::any maybe?
using Component_Create_Function = ID(*)(std::vector<Byte>&, Entity_Handle, Component_Base*);
using Component_Delete_Function = void (*)(Component_Base*);


// create a new id for components
ID Make_ID();

struct Component_Base {
    Entity_Handle entity;
};

template <class TComponent>
struct Component : public Component_Base {
    static const ID Id;
    static const std::size_t Mem_Size;
    static const Component_Create_Function Create_Func;
    static const Component_Delete_Function Delete_Func;
};

// create a new component by copying from a 'blueprint'
template <class TComponent>
ID CreateComponent(std::vector<Byte>& memory, Entity_Handle entity, Component_Base* blueprint)
{
    // allocate memory for component
    u32 index = memory.size();
    memory.resize(index + TComponent::Mem_Size);

    // don't allocate on the heap, but in the already allocated 'memory'!
    TComponent* component = new (&memory[index]) TComponent(*(TComponent*)blueprint);
    component->entity = entity;
    return index;
}

template <class TComponent>
void DeleteComponent(Component_Base* c)
{
    assert(c != nullptr);

    TComponent* component = (TComponent*)c;
    assert(component != nullptr); // typecast shouldn't fail!
    component-> ~TComponent();
}


// static init
template <class TComponent>
const ID Component<TComponent>::Id(Make_ID());

template <class TComponent>
const std::size_t Component<TComponent>::Mem_Size(sizeof(TComponent));

template <class TComponent>
const Component_Create_Function Component<TComponent>::Create_Func(CreateComponent<TComponent>);

template <class TComponent>
const Component_Delete_Function Component<TComponent>::Delete_Func(DeleteComponent<TComponent>);

