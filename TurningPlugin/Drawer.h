#pragma once
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"

class Drawer
{
private:
    std::shared_ptr<GameWrapper> game;

    void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);

public:
    Drawer(std::shared_ptr<GameWrapper> game) : game(game) {};

    void draw(CanvasWrapper canvas);
};