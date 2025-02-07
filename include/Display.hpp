#pragma once
#include "graphics.hpp"
#include "Chip-8.hpp"
#include "Helper.hpp"
#include <iostream>
#include <cstdint>
#include <string>
#define TOTAL_PIXELS 64*32

//TODO add file selection through menus
class Chip8Display : public Graphics::Window
{
    private:
        bool pixel_states[TOTAL_PIXELS]{};
        unsigned int PixelSize;
        Graphics::Canvas * MainScreen;
        Graphics::Canvas * DebugScreen;
        uint16_t * current_instruction_ptr;
        uint8_t * Registers_ptr;
        uint16_t * IRegister_ptr;
        uint16_t * ProgramCounter_ptr;
        uint16_t * Stack_ptr;   
        uint8_t * StackPointer_ptr;   
        uint8_t * Memory_ptr;
        bool * Key_ptr;
        Chip8 * Cpu;
        //Maybe have the pointer to the chip instead of those

        void DebugDrawInstructions()
        {
            Vector2 instruction_pos = DebugScreen->GloabalToCanvasCoords(Vector2{0,0});
            std::string instruction_txt = "Current instruction: " + IntToHex(*current_instruction_ptr);
            DrawText(instruction_txt.data(), instruction_pos.x, instruction_pos.y, 16, WHITE);
        }
        void DebugDrawRegisters()
        {
            Vector2 IReg_pos = DebugScreen->GloabalToCanvasCoords(Vector2{0,20});
            std::string IReg_txt = "I Register: " + IntToHex(*IRegister_ptr);
            DrawText(IReg_txt.data(), IReg_pos.x, IReg_pos.y, 16, WHITE);
            std::string Reg_text;
            Vector2 RegPos = DebugScreen->GloabalToCanvasCoords(0, 20);
            unsigned int y = 0;
            for (uint8_t i = 0; i < 16; i++)
            {
                //FIXME the order is incorrect (doesn't really matter)
                if(i % 2 == 0)
                {
                    y++;
                }
                Reg_text = "Register[" + std::to_string(i) + "]: " + IntToHex(Registers_ptr[i]); 
                DrawText(Reg_text.data(), RegPos.x + 200 * ((i + 1) % 2), y * 20 + RegPos.y, 16, GREEN);
            }
        }
        void DebugDrawProgramCounter()
        {
            std::string Program_Counter_txt = "Program Counter: " + IntToHex(*ProgramCounter_ptr);
            Vector2 Program_Counter_pos = DebugScreen->GloabalToCanvasCoords(Vector2{0, 20 * 11});
            DrawText(Program_Counter_txt.data(), Program_Counter_pos.x, Program_Counter_pos.y, 16, WHITE);
            
        }
        void DebugDrawStack()
        {
            std::string Stack_Pointer_txt = "StackPointer: " + IntToHex(*StackPointer_ptr);
            Vector2 Stack_Pointer_Pos = DebugScreen->GloabalToCanvasCoords(0, 280);
            std::string Stack_text;
            Vector2 StackPos = DebugScreen->GloabalToCanvasCoords(0, 300);
            unsigned int y = 0;
            DrawText(Stack_Pointer_txt.data(), Stack_Pointer_Pos.x, Stack_Pointer_Pos.y, 16, WHITE);
            for (uint8_t i = 0; i < 16; i++)
            {
                //FIXME the order is incorrect (doesn't really matter)
                if(i % 2 == 0)
                {
                    y++;
                }
                Stack_text = "Stack[" + std::to_string(i) + "]: " + IntToHex(Stack_ptr[i]); 
                DrawText(Stack_text.data(), StackPos.x + 200 * ((i + 1) % 2), y * 20 + StackPos.y, 16, GREEN);
            }
        }

        void DebugDrawMemory()
        {
            std::string Memory_Pointer_txt = "Memory[" + IntToHex(*ProgramCounter_ptr) + "]: " + IntToHex(Memory_ptr[*ProgramCounter_ptr]);
            Vector2 Memory_Pointer_pos = DebugScreen->GloabalToCanvasCoords(Vector2{0, 480});
            DrawText(Memory_Pointer_txt.data(), Memory_Pointer_pos.x, Memory_Pointer_pos.y, 16, WHITE);

        }

        void DebugDrawKeys()
        {
            unsigned int y = 0;
            std::string KeyText;
            Vector2 KeyPos = DebugScreen->GloabalToCanvasCoords(0, 500);
            for (uint8_t i = 0; i < 16; i++)
            {
                //FIXME the order is incorrect (doesn't really matter)
                if(i % 2 == 0)
                {
                    y++;
                }
                KeyText = "Key[" + std::to_string(i) + "]: " + IntToHex(Key_ptr[i]); 
                DrawText(KeyText.data(), KeyPos.x + 200 * ((i + 1) % 2), y * 20 + KeyPos.y, 16, GREEN);
            }
        }
        void DrawDebug()
        {
            DebugDrawInstructions();
            DebugDrawRegisters();
            DebugDrawProgramCounter();
            DebugDrawStack();
            DebugDrawMemory();
            DebugDrawKeys();
        }
        void KeyPad()
        {
            for (int i = 49; i < 53; i++)
            {
                
                Cpu->SetKey(i-49, IsKeyDown(i));
            }
            Cpu->SetKey(4, IsKeyDown(KEY_Q));
            Cpu->SetKey(5, IsKeyDown(KEY_W));
            Cpu->SetKey(6, IsKeyDown(KEY_E));
            Cpu->SetKey(7, IsKeyDown(KEY_R));
            Cpu->SetKey(8, IsKeyDown(KEY_A));
            Cpu->SetKey(9, IsKeyDown(KEY_S));
            Cpu->SetKey(10, IsKeyDown(KEY_D));
            Cpu->SetKey(11, IsKeyDown(KEY_F));
            Cpu->SetKey(12, IsKeyDown(KEY_Z));
            Cpu->SetKey(13, IsKeyDown(KEY_X));
            Cpu->SetKey(14, IsKeyDown(KEY_C));
            Cpu->SetKey(15, IsKeyDown(KEY_V));
            
        }
        void Update(float dt) override
        {
            if (Cpu->ShouldExit() || IsKeyDown(KEY_ESCAPE))
            {
                CloseWindow();
            }
            if(IsKeyDown(KEY_UP))
            {
                MainScreen->IncrementY(-1);
            } else if(IsKeyDown(KEY_DOWN))
            {
                MainScreen->IncrementY(1);
            }
            if(MainScreen->GetPos().y < 0)
            {
                MainScreen->SetY(0);
            } else if (MainScreen->GetPos().y + MainScreen->GetHeight() > W_height)
            {
                MainScreen->SetY(W_height - MainScreen->GetHeight());
            }
            
            KeyPad();
            Cpu->Cycle();
        }
        
        void Draw() override
        {
            unsigned int x,y;
            for (unsigned int i = 0; i < TOTAL_PIXELS; i ++)
            {
                if(!pixel_states[i]) //If pixel is not on dont draw it
                {
                    continue;
                }                 
                //FIXME something does not work right
                x = i % 64;
                y = i / 64;
                Vector2 pix_pos = MainScreen->GloabalToCanvasCoords(x * PixelSize,y * PixelSize);
                DrawRectangle(pix_pos.x, pix_pos.y, PixelSize, PixelSize, WHITE);
            }
            MainScreen->DrawBorder();
            DrawDebug();
            DebugScreen->DrawBorder();
            DrawFPS(0,0);
        }
        void Init()
        {
            current_instruction_ptr = Cpu->GetInstructionPointer();
            Registers_ptr = Cpu->GetRegistersPointer();
            IRegister_ptr = Cpu->GetIRegisterPoitner();
            ProgramCounter_ptr = Cpu->GetProgramCounterPtr();
            StackPointer_ptr = Cpu->GetStackPointerPtr();
            Stack_ptr = Cpu->GetStackPtr();
            Memory_ptr = Cpu->GetMemoryPointer();
            Key_ptr = Cpu->GetKeyPtr();
            Cpu->SetDisplay(pixel_states);
        }
    public:
        Chip8Display(Chip8* CPU, unsigned int pixel_size = 10) : Window(1000, 700, "Display"), PixelSize(pixel_size)
        {
            Cpu = CPU;
            MainScreen = new Graphics::Canvas(64 * pixel_size,32 * pixel_size, Vector2{0,0});
            DebugScreen = new Graphics::Canvas(1000 - 64 * pixel_size, W_height, Vector2{(float)(64 * pixel_size) + 1, 0});
            Init();
            clear();
        }

        ~Chip8Display()
        {
            delete MainScreen;
            delete DebugScreen;
            delete Cpu;
            clear();
        }
        void clear()
        {
            for (int i = 0; i < TOTAL_PIXELS; i++)
            {
                pixel_states[i] = false;
            }
        }

};