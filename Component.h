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


struct Component_Base {

    // inner type for easier managment
    struct Allocation_Data {
        Component_Create_Function create_func;
        Component_Delete_Function delete_func;
        std::size_t               size;
    };

    // this will point to the actual entity
    Entity_Handle entity;

    // register system
    static ID register_component(Component_Create_Function create_func, Component_Delete_Function delete_func, std::size_t size);
    static std::vector<Allocation_Data>* types;

    // accessors
    inline static Component_Create_Function create_function_of(ID id) { return (*types)[id].create_func; }
    inline static Component_Delete_Function delete_function_of(ID id) { return (*types)[id].delete_func; }
    inline static std::size_t size_of(ID id) { return (*types)[id].size; }
    inline static bool is_valid(ID id) { return id < types->size(); }
};

template <class T>
struct Component : public Component_Base {
    static const ID Id;
    static const std::size_t Mem_Size;
    static const Component_Create_Function Create_Func;
    static const Component_Delete_Function Delete_Func;
};

// create a new component by copying from a 'blueprint'
template <class T>
ID CreateComponent(std::vector<Byte>& memory, Entity_Handle entity, Component_Base* blueprint)
{
    assert(entity    != nullptr);
    assert(blueprint != nullptr);

    // allocate memory for component
    u32 index = memory.size();
    memory.resize(index + T::Mem_Size);

    // don't allocate on the heap, but in the already allocated 'memory'!
    T* component = new (&memory[index]) T(*(T*)blueprint);
    component->entity = entity;
    return index;
}

template <class T>
void DeleteComponent(Component_Base* c)
{
    assert(c != nullptr);

    T* component = (T*)c;
    assert(component != nullptr); // typecast shouldn't fail!
    component-> ~T();
}


// static init
template <class T>
const ID Component<T>::Id(Component_Base::register_component(CreateComponent<T>, DeleteComponent<T>, sizeof(T)));

template <class T>
const std::size_t Component<T>::Mem_Size(sizeof(T));

template <class T>
const Component_Create_Function Component<T>::Create_Func(CreateComponent<T>);

template <class T>
const Component_Delete_Function Component<T>::Delete_Func(DeleteComponent<T>);



// this empty component is only here to check compiler issues!
/// TODO remove if the first real components are in the program
struct Test_Component : public Component<Test_Component> {
    /*does nothing!*/
};