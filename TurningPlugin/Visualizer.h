#pragma once

#include "Drawer.h"

class Configuration;

class BoxDrawing : public Drawing
{
    virtual void draw(CanvasWrapper canvas);
};

class DirectionDrawing : public Drawing
{
    Rotator goalRot;
    
    // DirectionDrawing(Configuration* config, Rotator rot) : goalRot(rot), Drawing(config) {};
    
    virtual void draw(CanvasWrapper canvas);
};

class Visualizer
{
public:
    Configuration* config;

    Drawing* box;

    Visualizer(Configuration* config) : config(config) {};

    void render();
};