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

using ID   = u32;

using Byte = u8;
using Bytes = std::vector<Byte>;


// macros
#define for_size(container) for (u32 n = 0; n < std::size(container); ++n)