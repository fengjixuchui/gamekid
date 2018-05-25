#pragma once
#include "GameKid/cpu/instruction.h"
#include "opcodes/imm_to_reg_opcode.h"
#include "opcodes/reg_to_reg_opcode.h"
#include "opcodes/reg_to_mem_opcode.h"

class ld_instruction : public instruction
{
private:
    std::vector<imm_to_reg_opcode> _imm_to_reg;
    std::vector<reg_to_reg_opcode> _reg_to_reg;
    std::vector<reg_to_mem_opcode> _reg_to_mem;
public:
    explicit ld_instruction(cpu& cpu)
        : instruction(cpu, "ld")
    {
        add_imm_to_reg(0x3E, cpu.regs.A);
        add_imm_to_reg(0x06, cpu.regs.B);
        add_imm_to_reg(0x0E, cpu.regs.C);
        add_imm_to_reg(0x16, cpu.regs.D);
        add_imm_to_reg(0x1E, cpu.regs.E);
        add_imm_to_reg(0x26, cpu.regs.H);
        add_imm_to_reg(0x2E, cpu.regs.L);
        
        add_reg_to_reg_opcodes(0x78, cpu.regs.A);
        add_reg_to_reg_opcodes(0x40, cpu.regs.B);
        add_reg_to_reg_opcodes(0x48, cpu.regs.C);
        add_reg_to_reg_opcodes(0x50, cpu.regs.D);
        add_reg_to_reg_opcodes(0x58, cpu.regs.E);
        add_reg_to_reg_opcodes(0x60, cpu.regs.H);
        add_reg_to_reg_opcodes(0x68, cpu.regs.L);

        // Move from Register To Memory

        add_reg_to_mem(0x77, cpu.regs.A, cpu.regs.HL);
        add_reg_to_mem(0x70, cpu.regs.B, cpu.regs.HL);
        add_reg_to_mem(0x71, cpu.regs.C, cpu.regs.HL);
        add_reg_to_mem(0x72, cpu.regs.D, cpu.regs.HL);
        add_reg_to_mem(0x73, cpu.regs.E, cpu.regs.HL);
        add_reg_to_mem(0x74, cpu.regs.H, cpu.regs.HL);
        add_reg_to_mem(0x75, cpu.regs.L, cpu.regs.HL);


        // Move A Register to BC, DE
        add_reg_to_mem(0x02, cpu.regs.A, cpu.regs.BC);
        add_reg_to_mem(0x12, cpu.regs.A, cpu.regs.DE);
    }

    void reg_to_imm(byte* reg_address)
    {
        *reg_address = _cpu.mem.load_byte(_cpu.PC + 1);
    }

    void add_imm_to_reg(byte op_value, const reg8& reg)
    {
        _imm_to_reg.push_back(imm_to_reg_opcode(_cpu, reg, op_value));
    }

    void add_reg_to_reg_opcodes(byte b_value, const reg8& dst)
    {
        add_reg_to_reg(b_value-7, _cpu.regs.A, dst);
        add_reg_to_reg(b_value+0, _cpu.regs.B, dst);
        add_reg_to_reg(b_value+1, _cpu.regs.C, dst);
        add_reg_to_reg(b_value+2, _cpu.regs.D, dst);
        add_reg_to_reg(b_value+3, _cpu.regs.E, dst);
        add_reg_to_reg(b_value+4, _cpu.regs.H, dst);
        add_reg_to_reg(b_value+5, _cpu.regs.L, dst);
    }

    void add_reg_to_reg(byte val, const reg8& src, const reg8& dst)
    {
        _reg_to_reg.push_back(reg_to_reg_opcode(_cpu, val, src, dst));
    }

    void add_reg_to_mem(byte val, const reg8& src, const reg16& dst)
    {
        _reg_to_mem.push_back(reg_to_mem_opcode(_cpu, src, dst, val));
    }


    std::vector<byte> parse(const std::vector<std::string>& operands) override
    {
        // leave parsing for later
        return {};
    }
};
