#include "pch.h"
#include <gamekid/memory/memory.h>
#include <gamekid/memory/memory_map.h>


TEST(MEMORY, ECHO_INTERNAL_MEMO) {
    gamekid::memory::memory m({});
    
    for (int offset=0; offset<0x1e00; ++offset) {
        const word a_address = gamekid::memory::memory_map::internal_ram_8kb + offset;
        const word b_address = gamekid::memory::memory_map::internal_ram_8kb_echo + offset;

        m.store_byte(a_address, 100);
        ASSERT_EQ(100, m.load_byte(b_address));

        m.store_byte(b_address, 200);
        ASSERT_EQ(200, m.load_byte(a_address));

        ++offset;
    }
}