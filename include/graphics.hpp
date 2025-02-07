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
            Color border_color = VIOLET;
            virtual void _Draw(){}
                       
            bool IsInBounds(Vector2 obj_pos)
            {
                return (Pos.x <= obj_pos.x & obj_pos.x <= Pos.x + Width) & (Pos.y <= obj_pos.y & obj_pos.y <= Pos.y + Height);
            }
            
        public:
            Canvas(unsigned int c_width, unsigned int c_height, Vector2 c_pos) : Width(c_width), Height(c_height), Pos(c_pos){}
            ~Canvas(){}
            
            void DrawBorder()
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
    
} // namespace Graphics