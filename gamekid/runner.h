#pragma once
#include "system.h"
#include "cpu/cpu.h"
#include "cpu/instruction_set.h"
#include "cpu/opcode_decoder.h"
#include <set>
#include "gamekid.tests/test_rom_map.h"

namespace gamekid {

    class runner {
    private:
        rom::cartridge _cart;
        io::video::lcd _lcd;
        std::unique_ptr<rom::rom_map> _rom_map;
        memory::gameboy_memory_map _memory_map;
        system _system;
        cpu::instruction_set _set;
        cpu::opcode_decoder _decoder;
        std::set<word> _breakpoints;
    public:
        explicit runner(rom::cartridge&& rom);

        const std::set<word>& breakpoints() const {
            return _breakpoints;
        }

        std::vector<std::string> list(word address, word count);
        void add_breakpoint(word address);
        void run_until_break();
        void next();
        void run();
        cpu::cpu& cpu();
        std::vector<byte> dump(word address_to_view, word length_to_view);
        void delete_breakpoint(word breakpoint_address);
        void delete_all_breakpoints();
    };

}
