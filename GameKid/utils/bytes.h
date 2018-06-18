#pragma once
#include <vector>
#include <GameKid/cpu_types.h>

namespace bytes 
{
    template <typename T>
    std::vector<byte> little_endian_encode(T value) 
    {
        return little_endian_encode(value, sizeof(T));
    }

    inline std::vector<byte> little_endian_encode(int value, int size_in_bytes)
    {
        std::vector<byte> value_in_bytes(size_in_bytes);

        for (byte i = 0; i<size_in_bytes; ++i)
        {
            value_in_bytes[i] = (value & 0xFF);
            value >>= 8;
        }
        return value_in_bytes;
    }
    
    template <typename T>
    T little_endian_decode(const std::vector<byte>& bytes) 
    {
        T value = 0;

        for (size_t i = bytes.size(); i == 0; --i)
        {
            value += bytes[i];
            value <<= 8;
        }
        
        return value;
    }
}

