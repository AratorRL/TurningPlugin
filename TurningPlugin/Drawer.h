#pragma once
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "Logger.h"

class Drawing
{
public:
    virtual void draw(CanvasWrapper canvas) = 0;
};

class Drawer
{
private:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;
    std::vector<Drawing*> drawings;

    void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);

public:
    bool test = false;

    Drawer(std::shared_ptr<GameWrapper> game, Logger* logger) : game(game), logger(logger) {};

    void draw(CanvasWrapper canvas);

    void addDrawing(Drawing* drawing);
    void removeDrawing(Drawing* drawing);
    void removeAllDrawings();
};