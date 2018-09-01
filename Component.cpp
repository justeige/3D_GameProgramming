#include "Component.h"

static u32 Global_Component_ID = 0;

ID Make_ID()
{
    return Global_Component_ID++;
}