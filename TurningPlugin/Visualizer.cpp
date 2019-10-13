#include "Visualizer.h"
#include "utils.h"

void BoxDrawing::draw(CanvasWrapper canvas)
{
    canvas.SetPosition(Vector2 { 100, 100 });
    canvas.DrawBox(Vector2 { 200, 200 });
}

void DirectionDrawing::draw(CanvasWrapper canvas)
{
    //Vector ballLoc = util::getBallLocation(config->game);

    //Vector2 ballProj = canvas.Project(ballLoc);

    //// drawThiccLine(cw, carProj, ballProj);

    //canvas.SetColor(255, 0, 0, 255);
    //int width = 30;
    //Vector2 ballInd1Start = canvas.Project({ ballLoc.X - width, ballLoc.Y, 2 });
    //Vector2 ballInd1End = canvas.Project({ ballLoc.X - width, ballLoc.Y + 400, 2 });

    //Vector2 ballInd2Start = canvas.Project({ ballLoc.X + width, ballLoc.Y, 2 });
    //Vector2 ballInd2End = canvas.Project({ ballLoc.X + width, ballLoc.Y + 400, 2 });

    ////drawThiccLine(canvas, ballInd1Start, ballInd1End);
    ////drawThiccLine(canvas, ballInd2Start, ballInd2End);
}

void Visualizer::render()
{
    if (!box)
    {
        box = new BoxDrawing();
        // drawer->addDrawing(box);
    }
}