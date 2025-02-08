#pragma once

#include <raylib.h>
#include <string>

namespace Graphics
{
    //Acts like a window withing the window that the gui happens
    class Canvas
    {
        protected:
            unsigned int Width;
            unsigned int Height;
            Vector2 Pos;
            virtual void _Draw(){}
                       
            
            
        public:
            Canvas(unsigned int c_width, unsigned int c_height, Vector2 c_pos) : Width(c_width), Height(c_height), Pos(c_pos){}
            ~Canvas(){}
            
            void DrawBorder(Color border_color = VIOLET)
            {
                //TopLeft -> Bottom Left
                DrawLine(Pos.x, Pos.y, Pos.x, Pos.y + Height, border_color);

                //Bottomleft -> bottom Right
                DrawLine(Pos.x, Pos.y + Height, Pos.x + Width, Pos.y + Height, border_color);

                //Bottom Right -> Top right
                DrawLine(Pos.x + Width, Pos.y + Height, Pos.x + Width, Pos.y, border_color);

                //Topright -> Top Left
                DrawLine(Pos.x + Width, Pos.y, Pos.x, Pos.y, border_color);
            }
            Vector2 GloabalToCanvasCoords(Vector2 obj_pos) const
            {
                Vector2 new_pos = {obj_pos.x + Pos.x, obj_pos.y + Pos.y};
                return new_pos;
            }
            Vector2 GloabalToCanvasCoords(float x, float y) const
            {
                Vector2 new_pos = {x + Pos.x, y + Pos.y};
                return new_pos;
            }
            Vector2 FliHorizontally(Vector2 obj_pos)
            {
                Vector2 new_pos = {Width - obj_pos.x, obj_pos.y};
                return new_pos;
            }

            bool IsInBounds(Vector2 obj_pos)
            {
                return (Pos.x <= obj_pos.x & obj_pos.x <= Pos.x + Width) & (Pos.y <= obj_pos.y & obj_pos.y <= Pos.y + Height);
            }

            //Setters

            void IncrementX(int step = 1)
            {
                Pos.x += step;
            }
            void IncrementY(int step = 1)
            {
                Pos.y += step;
            }
            void SetY(int Y)
            {
                Pos.y = Y;
            }
            
            void ChangePos(float x, float y)
            {
                Pos.x = x; Pos.y = y;
            }

            void ChangePos(Vector2 new_pos)
            {
                Pos = new_pos;
            }

            //Getters

            Vector2 GetPos()
            {
                return Pos;
            }
            unsigned int GetHeight()
            {
                return Height;
            }

    };
    class Window
    {
        protected:
            unsigned int W_height;
            unsigned int W_Width;
            virtual void Draw()
            {

            }
            virtual void Update(float dt)
            {

            }
        public:
            Window(unsigned int w_width, unsigned int w_height,const char * w_title, unsigned int FPS = 60u) : W_Width(w_width), W_height(w_height)
            {
                InitWindow(w_width, w_height, w_title);
                SetTargetFPS(FPS);
            }
            ~Window() {}

            void operator()()
            {
                while (! WindowShouldClose())
                {
                    BeginDrawing();
                        Update(GetFrameTime());
                        ClearBackground(BLACK);
                        Draw();
                    EndDrawing();
                }
            }
    };
    class TextInput
    {
        private:
            std::string input_field_text;
            bool focused = false;
            Color focused_border_color{0, 255, 255, 255}; //CYAN
            Color unfocused_border_color{51, 51, 51, 255}; //GREY

            Canvas TextInputCanvas{16 * 32, 16, Vector2{0,0}};

            void HandleInput()
            {   
                if(input_field_text.size() <= 32)
                {
                    for (int i = 97; i <= 122; i++)
                    {
                        if(IsKeyPressed(i - 32))
                        {
                            input_field_text += (char)(i - IsKeyDown(KEY_LEFT_SHIFT) * 32);
                        }
                    }
                    for (int i = KEY_ZERO; i <= KEY_NINE; i++)
                    {
                        if(IsKeyPressed(i))
                        {
                            input_field_text += (char)(i);
                        }
                    }
                    
                }

                if(input_field_text.size() <= 0)
                {
                    return;
                }

                //TODO add a delay so it does not remove that fast
                if(IsKeyDown(KEY_BACKSPACE))
                {
                    input_field_text.pop_back();
                }
                
            }
        public:
            TextInput(Vector2 pos)
            {
                ChangePos(pos);
            }
            ~TextInput(){}

            void Draw()
            {
                //Draw Input Field (border)
                focused? TextInputCanvas.DrawBorder(focused_border_color) : TextInputCanvas.DrawBorder(unfocused_border_color);
                //Draw text
                Vector2 pos = TextInputCanvas.GloabalToCanvasCoords(0,0);
                DrawText(input_field_text.data(), pos.x, pos.y, 16, WHITE);
            }

            void Update()
            {
                //Check if is focused
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && TextInputCanvas.IsInBounds(GetMousePosition()))
                {
                    focused = true;

                } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    focused = false;
                }
                //Handle input if its focused
                if(focused)
                {
                    HandleInput();
                }
            }

            

            //Setters

            void ChangePos(float x, float y)
            {
                TextInputCanvas.ChangePos(x, y);
            }

            void ChangePos(Vector2 new_pos)
            {
                TextInputCanvas.ChangePos(new_pos);
            }

            //Getters

            std::string GetFilePath() const
            {
                if (input_field_text.length() == 0)
                {
                    return std::string("null");
                }
                return "ROMs/" + input_field_text + ".ch8";
            }

            bool IsFocused(){return focused;}
            
    };
} // namespace Graphics