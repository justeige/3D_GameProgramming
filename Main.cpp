//#include "Common.h"
#include "Vector.h"
#include "ECS.h"


struct TestComponent1 : public Component<TestComponent1> {
    int x, y, z;
} pos_test;

struct TestComponent2 : public Component<TestComponent2> {
    const char* label;
} label_test;

int main()
{
#ifdef _DEBUG
    // check entity-component-system
    ECS ecs;
    ecs.make_from(pos_test, label_test);
    auto current_components = ecs.m_components;
    auto current_entities   = ecs.m_entities;

    assert(current_components.size() == 2);
    assert(current_entities.size() == 1);
#endif // DEBUG


    return 0;
}