#include "Visualizer.h"

void BoxDrawing::draw(CanvasWrapper canvas)
{
    canvas.SetPosition(Vector2 { 100, 100 });
    canvas.DrawBox(Vector2 { 200, 200 });
}

void Visualizer::render()
{
    if (!box)
    {
        box = new BoxDrawing();
        drawer->addDrawing(box);
    }
}