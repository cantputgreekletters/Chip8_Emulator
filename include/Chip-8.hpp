#pragma once

#include <cstdint>
#include <iostream>
#include <math.h>
#include <fstream>
#include "Helper.hpp"
#include <ostream>

const char * LOG_FILE_PATH = "log.txt";

class Chip8
{
    private:
        uint8_t Registers[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //V0, V1, V2, ... , V9, VA, VB, ..., VF 16 8-bit Registers
        uint16_t IRegister = 0; //This register is generally used to store memory addresses, so only the lowest (rightmost) 12 bits are usually used.
        //4K Bytes of Memory
        uint8_t Memory[4096]{};
        uint16_t ProgramCounter = 0x200;    //16-bit Program Counter
        uint16_t Stack[16]{};           //16-level Stack
        bool Keypad[16]{};
        uint8_t StackPointer = 0;       //8-bit Stack Pointer
        uint8_t DelayTimer = 60;        //8-bit Delay Timer
        uint8_t SoundTimer = 60;        //8-bit Sound Timer
        //16 Input Keys
        //64x32 Monochrome Display Memory
        
        uint8_t opcode;
        uint16_t instruction;
        std::string LOGTXT;
        const unsigned int FONTSET_SIZE = 80;
        bool * display_pixels; //[64 * 32]
        bool EXIT = false; //TODO implement exit

        bool WillLog = false;
        bool HasLoadedROM = false;

        uint8_t fontset[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };


        //Find appropriate function based on the opcode and execute it
        void DecodeAndExecute()
        {
            switch (opcode)
            {
                case 0:
                    if((instruction & 0x00E0) || (instruction & 0x00EE))
                    {
                        OpNoParam();
                        break;
                    }
                case 1:
                case 2:
                case 0xA:
                case 0xB:
                    OpAddress();
                    break;
                case 3:
                case 4:
                case 6:
                case 7:
                case 0xC:
                    OpVxByte();
                    break;
                case 5:
                case 8:
                case 9:
                    OpVxVy();
                    break;
                case 0xD:
                    OpDxyn();
                    break;
                case 0xE:
                case 0xF:
                    OpVx();
                    break;
                default:
                    std::cout << std::string("Critical Error: Could not find instruction: ") + IntToHex(instruction) + std::string("\n");
                    if(WillLog)
                    {
                        LOGTXT += std::string("Critical Error: Could not find instruction: ") + IntToHex(instruction) + std::string("\n");
                    }
                    EXIT = true;
                    return;
                    break;
            }
        }

        void OpNoParam()
        {
            if(instruction == 0x00E0) //clear the display
            {
                clearScreen();
            } else if (instruction == 0x00EE) //Return from a subroutine.
            {   //The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
                if(StackPointer == 0)
                {
                    std::cout << "Stack underflow!\n";
                    if(WillLog)
                    {
                        LOGTXT += "STACK UNDERFLOW\n";
                    }
                    EXIT = true;
                }
                StackPointer --;
                ProgramCounter = Stack[StackPointer];
                /* code */
            } else
            {
                std::cout << "Invalid no param instruction!\n";
                if(WillLog)
                {
                    LOGTXT += "Invalid no param instruction!\n";
                }
                EXIT = true;
            }
            
        }
        void OpAddress()
        {
            uint16_t address = instruction & 0x0FFFu;
            //address should always be like 0000 nnnn nnnn nnnn, where the n's are the address
            switch (opcode)
            {
            case 0: //0nnn 
                /* code */
                break;
            case 1: //1nnn Jp or Jump
                /* Jump to location nnn.
                The interpreter sets the program counter to nnn.*/
                ProgramCounter = address;
                break;
            case 2: //2nnn CALL
                /*Call subroutine at nnn.
                The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.*/    
                if(StackPointer >= 16)
                {
                    std::cout << "Stack overflow!\n";
                    if(WillLog)
                    {
                        LOGTXT += "STACK OVERFLOW\n";
                    }
                    EXIT = true;
                }
                Stack[StackPointer] = ProgramCounter;
                StackPointer ++;
                ProgramCounter = address;
                break;
            case 0xA: //Annn  LD
                /* Set I = nnn.
                The value of register I is set to nnn.
                */
                IRegister = address;
                break;
            case 0xB: //Bnnn Jp
                /*Jump to location nnn + V0.
                The program counter is set to nnn plus the value of V0.
                */
                ProgramCounter = address + Registers[0];
                break;
            default:
                std::cout << "Error on OpAddress\n";
                if(WillLog)
                {
                    LOGTXT += "Error on OpAddress\n";
                }
                EXIT = true;
                break;
            }
        }
        void OpVxByte()
        {
            uint8_t x = (instruction & 0x0F00) >> 8u; //the result is 4 bits
            uint8_t Byte = (instruction & 0x00FF);
            //x is Vx dont make a mistake !!!
            switch (opcode)
            {
            case 0x03: //3xkk SE
                /*Skip next instruction if Vx = kk.
                The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.*/
                if(Registers[x] == Byte)
                {
                    ProgramCounter += 2;
                }
                break;
            case 0x04: //4xkk SNE
                /*Skip next instruction if Vx != kk.
                The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.*/
                if(Registers[x] != Byte)
                {
                    ProgramCounter += 2;
                }
                break;
            case 0x06: //6xkk LD
                /*Set Vx = kk.
                The interpreter puts the value kk into register Vx.*/
                Registers[x] = Byte;
                break;
            case 0x07: //7xkk ADD
                /*Set Vx = Vx + kk.
                Adds the value kk to the value of register Vx, then stores the result in Vx. */
                Registers[x] += Byte;
                break;
            case 0x0C: //Cxkk RND
                /* Set Vx = random byte AND kk.
                The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
                The results are stored in Vx. See instruction 8xy2 for more information on AND.*/
                Registers[x] = (int)(((float)(rand()) / RAND_MAX) * 255) & Byte;
                //TODO check this
                break;
            default:
                std::cout << "Error on OpOxkkk\n";
                if(WillLog)
                {
                    LOGTXT += "Error on OpOxkkk\n";
                }
                EXIT = true;
                break;
            }
        }
        void OpVxVy()
        {
            uint8_t x = (instruction & 0x0F00) >> 8u; //result is 4 bits
            uint8_t y = (instruction & 0x00F0) >> 4u; //result is 4 bits
            uint8_t last_digit = (instruction & 0x000F);
            switch (opcode)
            {
            case 0x8:
                switch (last_digit)
                {
                case 0x0: //0x8xy0
                    /*Set Vx = Vy.
                    Stores the value of register Vy in register Vx.*/
                    Registers[x] = Registers[y];
                    break;
                case 0x1: //0x8xy1
                    /*Set Vx = Vx OR Vy.
                    Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
                    A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
                    then the same bit in the result is also 1. Otherwise, it is 0. */
                    Registers[x] = Registers[x] | Registers[y];
                    break;
                case 0x2: //0x8xy2
                    /*Set Vx = Vx AND Vy.
                    Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. 
                    A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, 
                    then the same bit in the result is also 1. Otherwise, it is 0.*/
                    Registers[x] = Registers[x] & Registers[y];
                    break;
                case 0x3: //0x8xy3
                    /*Set Vx = Vx XOR Vy.
                    Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. 
                    An exclusive OR compares the corrseponding bits from two values, and if the bits are not 
                    both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. */
                    Registers[x] = Registers[x] ^ Registers[y];
                    break;
                case 0x4: //0x8xy4
                    /* Set Vx = Vx + Vy, set VF = carry.
                    The values of Vx and Vy are added together. If the result is greater than 
                    8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result 
                    are kept, and stored in Vx. */
                    Registers[0xF] = Registers[x] + Registers[y] > 255;
                    Registers[x] = (Registers[x] + Registers[y]) & 0xFFu;   
                    break;
                case 0x5: //0x8xy5
                    /*Set Vx = Vx - Vy, set VF = NOT borrow.
                    If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
                    and the results stored in Vx.*/
                    Registers[0xF] = Registers[x] > Registers[y]; //May need to be >= instead of >
                    Registers[x] -= Registers[y];
                    break;
                case 0x6: //0x8xy6
                    /*Set Vx = Vx SHR 1.
                    If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. 
                    Then Vx is divided by 2.*/
                    Registers[0xF] = Registers[x] & 0b1;
                    Registers[x] >>= 1;
                    break;
                case 0x7: //0x8xy7
                    /*Set Vx = Vy - Vx, set VF = NOT borrow.
                    If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, 
                    and the results stored in Vx.*/

                    Registers[0xF] = Registers[y] > Registers[x];
                    Registers[x] = Registers[y] - Registers[x];
                    break;
                case 0xE: //0x8xyE
                    /*Set Vx = Vx SHL 1.
                    If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
                    Then Vx is multiplied by 2.*/

                    //most sagnificant bit is left-most bit
                    Registers[0xF] = Registers[x] >> 7;
                    Registers[x] <<= 1;
                    break;
                default:
                    std::cout << "Error: on 8xyn could not find instruction with appropriate last digit\n";
                    if(WillLog)
                    {
                        LOGTXT += "Error: on 8xyn could not find instruction with appropriate last digit\n";
                    }
                    EXIT = true;
                    return;
                    break;
                }
                break;
            case 0x5:
                //Only one option: 5xy0
                /*Skip next instruction if Vx = Vy.
                The interpreter compares register Vx to register Vy, and if they are equal,
                increments the program counter by 2.*/
                if(Registers[x] == Registers[y])
                {
                    ProgramCounter += 2;
                }
                break;
            case 0x9:
                //Only one option: 0x9xy0
                /*Skip next instruction if Vx != Vy.
                The values of Vx and Vy are compared, and if they are not equal, 
                the program counter is increased by 2.*/
                if(Registers[x] != Registers[y])
                {
                    ProgramCounter += 2;
                }
                break;
            default:
                std::cout << "Error: on Oxyn could not find instruction with appropriate OpCode\n";
                if(WillLog)
                {
                    LOGTXT += "Error: on Oxyn could not find instruction with appropriate OpCode\n";
                }
                EXIT = true;
                return;
                break;
            }
        }

        void OpVx()
        {
            uint8_t x = (instruction & 0x0F00) >> 8u; //result is 4 bits
            uint8_t last_digits = (instruction & 0x00FF);
            switch (opcode)
            {
            case 0xF:
                switch (last_digits)
                {
                case 0x07: //Fx07
                    /*Set Vx = delay timer value.
                    The value of DT is placed into Vx.*/
                    Registers[x] = DelayTimer;
                    break;
                case 0x0A: //Fx0A
                    /*Wait for a key press, store the value of the key in Vx.
                    All execution stops until a key is pressed, then the value of that key is stored in Vx.*/
                    std::cout << "Waiting for input...\n";
                    
                    {
                        bool found = false;
                        for (uint8_t i = 0; i < 16; i++)
                        {
                            if(Keypad[i])
                            {
                                Registers[x] = i;
                                found = true;
                                break;
                            }
                        }
                        if(!found)
                        {
                            ProgramCounter -= 2;
                        } else {
                            std::cout << "Should exit!!!!!!\n";
                        }
                    }
                    //TODO check again Fx0A
                    break;
                case 0x15: //Fx15
                    /*Set delay timer = Vx.
                    DT is set equal to the value of Vx.*/
                    DelayTimer = Registers[x];
                    break;
                case 0x18: //Fx18
                    /*Set sound timer = Vx.
                    ST is set equal to the value of Vx.*/
                    SoundTimer = Registers[x];
                    break;
                case 0x1E: //Fx1E
                    /*Set I = I + Vx.
                    The values of I and Vx are added, and the results are stored in I.*/
                    IRegister += Registers[x];
                    break;
                case 0x29: //Fx29
                    /*Set I = location of sprite for digit Vx.
                    The value of I is set to the location for the hexadecimal sprite 
                    corresponding to the value of Vx. See section 2.4, Display, 
                    for more information on the Chip-8 hexadecimal font.*/
                    IRegister = 0x50 + (Registers[x] * 5);
                    break;
                case 0x33: //Fx33
                    /*Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at 
                    location in I, the tens digit at location I+1, and the ones digit at location I+2.*/
                    {
                        uint8_t value = Registers[x];
                        Memory[IRegister + 2] = value % 10;
                        value /= 10;
                        Memory[IRegister + 1] = value % 10;
                        value /= 10;
                        Memory[IRegister] = value % 10; 
                    }
                    

                    break;
                case 0x55: //Fx55
                    /*Store registers V0 through Vx in memory starting at location I.
                    The interpreter copies the values of registers V0 through Vx into memory,
                    starting at the address in I.*/
                    for (int i = 0; i <= x; i++)
                    {
                        Memory[IRegister + i] = Registers[i];
                    }

                    break;
                case 0x65: //Fx65
                    /*Read registers V0 through Vx from memory starting at location I.
                    The interpreter reads values from memory starting at location
                    I into registers V0 through Vx.*/
                    for (int i = 0; i <= x; i++)
                    {
                        Registers[i] = Memory[IRegister + i];
                    }
                    break;
                default:
                    std::cout << "Error: on Fxnn could not find instruction with appropriate last digits\n";
                    if(WillLog)
                    {
                        LOGTXT += "Error: on Fxnn could not find instruction with appropriate last digits\n";
                    }
                    EXIT = true;
                    return;
                    break;
                }
                break;
            case 0xE:
                switch (last_digits)
                {
                case 0x9E: // Ex9E
                    /*Skip next instruction if key with the value of Vx is pressed.
                    Checks the keyboard, and if the key corresponding to the value of Vx is 
                    currently in the down position, PC is increased by 2.*/
                    if(Keypad[x])
                    {
                        ProgramCounter += 2;
                    }
                    break;
                
                case 0xA1: // ExA1
                    /*Skip next instruction if key with the value of Vx is not pressed.
                    Checks the keyboard, and if the key corresponding to the value of Vx is 
                    currently in the up position, PC is increased by 2.*/
                    if(! Keypad[x])
                    {
                        ProgramCounter += 2;
                    }
                    break;
                default:
                    std::cout << "Error: on Exnn could not find instruction with appropriate last digits\n";
                    if(WillLog)
                    {
                        LOGTXT += "Error: on Exnn could not find instruction with appropriate last digits\n";
                    }
                    EXIT = true;
                    return;
                    break;
                    break;
                }
                break;
            default:
                    std::cout << "Error: on Oxnn could not find instruction with appropriate OpCode\n";
                    LOGTXT += "Error: on Oxnn could not find instruction with appropriate OpCode\n";
                    EXIT = true;
                    break;
                break;
            }
        }

        void OpDxyn()
        {
            /*Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed 
            as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels 
            to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates 
            of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, 
            Display, for more information on the Chip-8 screen and sprites.*/
            uint8_t x,y, byte;
            x = (instruction & 0x0F00) >> 8u;
            y = (instruction & 0x00F0) >> 4u;
            byte = (instruction & 0x000F);
            Registers[0xF] = DrawSprite(Registers[x], Registers[y], byte);
        }

        //Returns true if it causes some pixels to be erased
        bool DrawSprite(uint8_t x, uint8_t y, uint8_t sprite_height)
        {
            //FIXME something does not work right
            bool found = false;
            uint8_t SpriteByte, spritePixel;
            unsigned int screen_idx = 0;
            unsigned int SCREEN_HEIGHT = 32;
            unsigned int SCREEN_WIDTH = 64;
            for (unsigned int row = 0; row < sprite_height; row++)
            {
                SpriteByte = Memory[IRegister + row];

                for (unsigned int col = 0; col < 8; col++)
                {
                    spritePixel = SpriteByte & (0x80u >> col);
                    screen_idx = ((y + row) % SCREEN_HEIGHT) * SCREEN_WIDTH + ((x + col) % SCREEN_WIDTH);

                    if (spritePixel)
                    {
                        if (display_pixels[screen_idx])
                        {
                            found = true;
                        }

                        display_pixels[screen_idx] ^= 1;
                    }
                    
                }
            }
            return found;
        }
        void clearScreen()
        {
            for (int i = 0; i < 64 * 32; i++)
            {
                display_pixels[i] = false;
            }
        }

        void WriteToLog()
        {
            std::ofstream file(LOG_FILE_PATH, std::ofstream::out | std::ofstream::trunc);
            if(file.is_open())
            {
                file.write(LOGTXT.data(), LOGTXT.length());
            } else{
                std::cout << "Failed to write to log\n";
            }
        }
public:
    Chip8() 
    {
        for (int i = 0; i < 16; i++)
        {
            Keypad[i] = false;
            Stack[i] = 0;
            Memory[i] = 0;
            Registers[i] = 0u;
        }
        for (int i = 16; i < 4096; i++)
        {
            Memory[i] = 0;
        } 

        //Load fontset to memory
        for (unsigned int i = 0; i < FONTSET_SIZE; i++)
        {
            Memory[0x50 + i] = fontset[i];
        }
        
        
    }
    uint16_t * GetInstructionPointer()
    {
        return &instruction;
    }
    uint8_t * GetRegistersPointer()
    {
        return Registers;
    }
    uint16_t * GetIRegisterPoitner()
    {
        return & IRegister;
    }
    uint16_t * GetProgramCounterPtr()
    {
        return & ProgramCounter;
    }
    uint8_t * GetStackPointerPtr()
    {
        return & StackPointer;
    }
    uint16_t * GetStackPtr()
    {
        return Stack;
    }
    uint8_t * GetMemoryPointer()
    {
        return Memory;
    }
    bool * GetKeyPtr()
    {
        return Keypad;
    }
    void SetDisplay(bool DisplayPixels[64 * 32])
    {
        display_pixels = DisplayPixels;
    }
    void Cycle()
    {
        if(! HasLoadedROM)
        {
            return;
        }
        if(ProgramCounter < 0x200)
        {
            std::cout << "Program counter is less than 0x200\n";
            if(WillLog)
            {
                LOGTXT += "Program counter is less than 0x200\n";
            }
            EXIT = true;
        } else if (ProgramCounter > 0x600)
        {
            std::cout << "Program counter is more than 0x600\n";
            if(WillLog)
            {
                LOGTXT += "Program counter is more than 0x600\n";
            }
            EXIT = true;
        }
        //Fetch
        instruction = (Memory[ProgramCounter] << 8) | Memory[ProgramCounter + 1];
        //instruction example 00E0 in hex, 0000 0000 1110 0000 in binary
        //                                 ^--^ is opcode
        opcode = (instruction & 0xF000) >> 12;
        //from exaple op code should be 0000 - 0000 where the second half of the byte is the op code we need
        ProgramCounter += 2;
        
        if(WillLog)
        {
            LOGTXT += "Before Exec: C.I: " + IntToHex(instruction) + ", S.P: " + IntToHex(StackPointer)
            + ", P.C: " + IntToHex(ProgramCounter) + "\n";
        }
        //Decode
        DecodeAndExecute();
        if (DelayTimer > 0)
        {
            DelayTimer --;
        }
        if (SoundTimer > 0)
        {
            SoundTimer --;
        }
        if(WillLog)
        {
            LOGTXT += "After Exec: C.I: " + IntToHex(instruction) + ", S.P: " + IntToHex(StackPointer)
            + ", P.C: " + IntToHex(ProgramCounter) + "\n";
        }

    }
    void SetKey(uint8_t key_idx, bool state)
    {
        Keypad[key_idx] = state;
    }

    void UnloadROM()
    {
        for (int i = 0; i < 16; i++)
        {
            Keypad[i] = false;
            Stack[i] = 0;
            Memory[i] = 0;
            Registers[i] = 0u;
        }
        for (int i = 16; i < 4096; i++)
        {
            Memory[i] = 0;
        }
        IRegister = 0;
        ProgramCounter = 0x200;
        StackPointer = 0;
        DelayTimer = 60;
        SoundTimer = 60;
        //Clears screen
        for(unsigned int i = 0; i < 64*32; i++)
        {
            display_pixels[i] = false;
        }
    }

    void LoadROM(char const* filename)
    {
        const unsigned int START_ADDRESS = 0x200;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open())
        {
            std::streampos size = file.tellg();
            char* buffer = new char[size];

            // Go back to the beginning of the file and fill the buffer
            file.seekg(0, std::ios::beg);
            file.read(buffer, size);
            file.close();

            // Load the ROM contents into the Chip8's memory, starting at 0x200
            for (long i = 0; i < size; ++i)
            {
                Memory[START_ADDRESS + i] = buffer[i];
            }

            // Free the buffer
            delete[] buffer;

            HasLoadedROM = true;
        } else
        {
            HasLoadedROM = false;
            std::cout << "Failed to Load ROM with name: " << filename << '\n';
        }
        
    }
    bool ShouldExit()
    {
        return EXIT;
    }

    void EnableLogging()
    {
        WillLog = true;
    }
    ~Chip8()
    {
        if(WillLog)
        {
            WriteToLog();
        }
    }
};