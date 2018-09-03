#pragma once
// --------------------------------------------------
// all project common types/macros/functions
// --------------------------------------------------

#include <stdint.h>
#include <assert.h>
#include <vector>

// typedefs

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using i32 = int32_t;
using i64 = int32_t;

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