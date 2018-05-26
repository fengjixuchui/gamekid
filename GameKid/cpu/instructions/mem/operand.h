#pragma once
#include <string>
#include "GameKid/memory.h"
#include "GameKid/cpu.h"

class operand
{
public:
    virtual ~operand() = default;
    virtual std::string to_str(byte* next) = 0;
    virtual std::vector<byte> bytes(std::string operand) = 0;
};

template <typename T>
class source_operand : public operand
{
public:
    virtual T load() = 0;
};

template <typename T>
class dest_operand : public operand
{
public:
    virtual void store(T value) = 0;
};

class reg_mem_operand : public source_operand<byte>, public dest_operand<byte>
{
public:
    memory & _mem;
    reg16 _reg;

    reg_mem_operand(memory& mem, const reg16& reg) : _mem(mem), _reg(reg)
    {
    }


    byte load() override
    {
        return _mem.load_byte(_reg.get());
    }

    void store(byte s) override
    {
        _mem.store(_reg.get(), s);
    }

    std::string to_str(byte* next) override
    {
        return "[" + _reg.name + "]";
    }
    std::vector<byte> bytes(std::string operand) override
    {
        return {};
    }
};

template <typename T>
std::vector<byte> immidiate_bytes(std::string operand)
{
    int parsed = stoi(operand);

    if (parsed > sizeof(T))
    {
        throw std::exception("Operand value exeeds");
    }

    T real_value = (T)parsed;

    std::vector<byte> v(sizeof(T));

    for (byte i = 0; i<sizeof(T); ++i)
    {
        v.push_back(real_value & 0xFF);
        real_value >>= 8;
    }

    return v;
}

template <typename T>
class imm_operand : public source_operand<T>
{
private:
    cpu & _cpu;

public:
    explicit imm_operand(cpu& cpu) : _cpu(cpu) {}

    T load() override
    {
        return _cpu.mem.load<T>(_cpu.PC + 1);
    }

    std::string to_str(byte* next) override
    {
        T item = *(T*)next;
        return std::to_string(item);
    }

    std::vector<byte> bytes(std::string operand) override 
    {
        return immidiate_bytes<T>(operand);
    }
};

class imm_mem_operand : public source_operand<byte>, public dest_operand<byte>
{
private:
    cpu & _cpu;
public:
    explicit imm_mem_operand(cpu& cpu) : _cpu(cpu) {}

    word get_address()
    {
        return _cpu.mem.load_word_le(_cpu.PC + 1);
    }

    byte load() override
    {
        return _cpu.mem.load_byte(get_address());
    }

    std::string to_str(byte* next) override
    {
        const word address = *(word*)next;
        return std::to_string(address);
    }

    void store(byte value) override
    {
        _cpu.mem.store(get_address(), value);
    }

    std::vector<byte> bytes(std::string operand) override
    {
        return immidiate_bytes<word>(operand);
    }
};


class reg16_offset_operand : public source_operand<byte>
{
private:
    cpu & _cpu;
    reg16 _reg;
public:
    explicit reg16_offset_operand(cpu& cpu, const reg16& reg) : _cpu(cpu), _reg(reg) {}

    byte load() override
    {
        const byte offset = _cpu.mem.load_byte(_cpu.PC + 1);

        if (offset <= CHAR_MAX)
        {
            return  _reg.get() + offset;
        }

        return _reg.get() - (offset - CHAR_MAX);
    }

    std::string to_str(byte* next) override
    {
        const char value = *(char*)(next);

        if (value < 0)
        {
            return _reg.name + std::to_string(value);
        }
        
        return _reg.name + "+" + std::to_string(value);
    }

    std::vector<byte> bytes(std::string operand) override
    {
        size_t index_of_offset = operand.find("+");
        std::string offset = operand.substr(index_of_offset + 1);
        return immidiate_bytes<word>(offset);
    }
};

class reg8_operand : public source_operand<byte>, public dest_operand<byte>
{
private:
    reg8 _reg;
public:
    reg8_operand(const reg8& reg) : _reg(reg) {}

    byte load() override
    {
        return _reg.get();
    }

    std::string to_str(byte* next) override
    {
        return _reg.name;
    }

    void store(byte value) override
    {
        _reg.set(value);
    }

    std::vector<byte> bytes(std::string operand) override
    {
        return {};
    }
};

class reg16_operand : public source_operand<word>, public dest_operand<word>
{
private:
    reg16 _reg;
public:
    reg16_operand(const reg16& reg) : _reg(reg) {}

    word load() override
    {
        return _reg.get();
    }

    std::string to_str(byte* next) override
    {
        return _reg.name;
    }

    void store(word value) override
    {
        _reg.set(value);
    }
    std::vector<byte> bytes(std::string operand) override
    {
        return {};
    }
};

class c_mem_operand : public source_operand<byte>, public dest_operand<byte>
{
private:
    cpu & _cpu;
public:
    explicit c_mem_operand(cpu& cpu);

    byte load() override
    {
        const word address = _cpu.regs.C.get() + 0xFF00;
        return _cpu.mem.load_byte(address);
    }

    std::string to_str(byte* next) override
    {
        return "[c]";
    }

    void store(byte value) override
    {
        const word address = _cpu.regs.C.get() + 0xFF00;
        return _cpu.mem.store(address, value);
    }
    std::vector<byte> bytes(std::string operand) override
    {
        return {};
    }
};
