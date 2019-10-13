#include "Drawer.h"
#include "utils.h"

void Drawer::draw(CanvasWrapper canvas)
{
    if (!game->IsInFreeplay())
        return;

    if (this->test)
    {
        drawThiccLine(canvas, { 0, 0 }, { 500, 500 });

        return;
    }

    Vector ballLoc = util::getBallLocation(game);

    Vector2 ballProj = canvas.Project(ballLoc);

    // drawThiccLine(cw, carProj, ballProj);

    canvas.SetColor(255, 0, 0, 255);
    int width = 30;
    Vector2 ballInd1Start = canvas.Project({ ballLoc.X - width, ballLoc.Y, 2 });
    Vector2 ballInd1End = canvas.Project({ ballLoc.X - width, ballLoc.Y + 400, 2 });

    Vector2 ballInd2Start = canvas.Project({ ballLoc.X + width, ballLoc.Y, 2 });
    Vector2 ballInd2End = canvas.Project({ ballLoc.X + width, ballLoc.Y + 400, 2 });

    drawThiccLine(canvas, ballInd1Start, ballInd1End);
    drawThiccLine(canvas, ballInd2Start, ballInd2End);
}

void Drawer::drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end)
{
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            Vector2 s = { start.X + i, start.Y + j };
            Vector2 e = { end.X + i, end.Y + j };
            cw.DrawLine(s, e);
        }
    }
}

void Drawer::addDrawing(Drawing* drawing)
{
    drawings.push_back(drawing);
    logger->log("# drawings = " + to_string(drawings.size()));
}

void Drawer::removeDrawing(Drawing* drawing)
{
    drawings.erase(std::remove(drawings.begin(), drawings.end(), drawing));
    logger->log("# drawings = " + to_string(drawings.size()));
}

void Drawer::removeAllDrawings()
{
    drawings.clear();
}


void Drawing::draw(CanvasWrapper canvas)
{

}