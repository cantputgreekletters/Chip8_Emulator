#include "Chip-8.hpp"
#include "Display.hpp"

int main(int argc, char** argv)
{
    static Chip8 CPU;
    #if defined CPU_LOG
        CPU.EnableLogging();
    #endif
    //CPU.LoadROM("ROMs/chip8logo.ch8");
    static Chip8Display Display(&CPU);
    Display();
    return 0;
}