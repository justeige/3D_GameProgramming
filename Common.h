#pragma once
// --------------------------------------------------
// all project common types/macros/functions
// --------------------------------------------------

#include <stdint.h>
#include <assert.h>
#include <vector>

// typedefs

using uint = unsigned int;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i32 = int32_t;
using i64 = int64_t;

using ID   = u32;

using Byte = u8;
using Bytes = std::vector<Byte>;


// macros
#define for_size(var, container) for (u32 var = 0; var < std::size(container); ++var)

#define no_copy_and_assign(obj) \
    obj(obj const& other) = delete; \
    void operator=(obj const& other) = delete;

#define no_move_and_assign(obj) \
    obj(obj && other) = delete; \
    void operator=(obj && other) = delete;

#define not_in_use(param) (param)


// black magic macros for unique names
// src = https://stackoverflow.com/questions/2419650/c-c-macro-template-blackmagic-to-generate-unique-name
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE_NAME(x) CONCATENATE(x, __COUNTER__)


/// TODO move into own header if needed elsewhere
template <class Action>
struct Deferred_Action {
    Deferred_Action(Action&& a) : action(a) {}
    ~Deferred_Action() { action(); }
    Action action;
};
#define ON_EXIT(action) Deferred_Action MAKE_UNIQUE_NAME(deferred_action_)([&](){action;})