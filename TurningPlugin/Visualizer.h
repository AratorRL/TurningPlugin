#pragma once

#include "Drawer.h"

class BoxDrawing : public Drawing
{
    virtual void draw(CanvasWrapper canvas);
};

class Visualizer
{
public:
    std::shared_ptr<GameWrapper> game;
    Drawer* drawer;

    Drawing* box;

    Visualizer(std::shared_ptr<GameWrapper> game, Drawer* drawer) : game(game), drawer(drawer) {};

    void render();
};