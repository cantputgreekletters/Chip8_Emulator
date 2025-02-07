#include "Chip-8.hpp"
#include "Display.hpp"

int main(int argc, char** argv)
{
    Chip8 CPU;
    CPU.EnableLogging();
    CPU.LoadROM("ROMs/chip8logo.ch8");
    Chip8Display Display(&CPU);
    Display();
    return 0;
}