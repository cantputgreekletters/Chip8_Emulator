#pragma once

#include <cstdint>

//16 levels of 16 bit values
class Chip8Stack
{
private:
    uint16_t container[16]{};
public:
    Chip8Stack(/* args */) {}
    ~Chip8Stack() {}
};