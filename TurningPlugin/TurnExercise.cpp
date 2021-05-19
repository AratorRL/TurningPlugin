#define _USE_MATH_DEFINES
#include <math.h>
#include "TurnExercise.h"
#include "utils.h"

TurningRecording* TurnExercise::getCurrentRecording()
{
    return this->recording[currRecordingBuffer];
}

TurningRecording* TurnExercise::getLastRecording()
{
    return this->recording[1 - currRecordingBuffer];
}

void TurnExercise::swapRecordingBuffers()
{
    this->currRecordingBuffer = 1 - this->currRecordingBuffer;
}

void TurnExercise::saveSnapshot()
{
    CarWrapper car = util::getCar(game);
    Vector loc = car.GetLocation();
    Rotator rot = car.GetRotation();

    ControllerInput input = util::getCar(game).GetInput();

    TurningRecording* recording = this->getCurrentRecording();

    recording->snapshots.push_back({
        loc,
        rot,
        input.Throttle,
        input.Steer,
        (bool)input.HoldingBoost,
        (bool)input.Handbrake
    });
}

void TurnExercise::analyzeTurn(TurningRecording* rec)
{
    if (rec->snapshots.size() <= 0)
    {
        cvarManager->log("analyze: no snapshots");
        return;
    }

    TurningSnapshot firstSnap = rec->snapshots.front();
    float startAngle = -firstSnap.rotation.Yaw * M_PI / 32768 - M_PI / 2;

    int currentInput = -1;

    bool colorBoost = cvarManager->getCvar("turn_graph_boost").getBoolValue();
    bool colorPowerslide = cvarManager->getCvar("turn_graph_powerslide").getBoolValue();
    bool colorSteer = cvarManager->getCvar("turn_graph_steer").getBoolValue();

    for (int i = 0; i < rec->snapshots.size(); i++)
    {
        TurningSnapshot snap = rec->snapshots.at(i);
        Vector relativeLoc = snap.location - firstSnap.location;
        Vector2 vec = util::rotateVec2(Vector2F{ relativeLoc.X, relativeLoc.Y }, startAngle);
        rec->points.push_back(vec);

        if (vec.X < rec->pbound.minX)
            rec->pbound.minX = vec.X;
        if (vec.X > rec->pbound.maxX)
            rec->pbound.maxX = vec.X;
        if (vec.Y < rec->pbound.minY)
            rec->pbound.minY = vec.Y;
        if (vec.Y > rec->pbound.maxY)
            rec->pbound.maxY = vec.Y;

        int input = INPUT_NONE;
        if (snap.boost && colorBoost)
            input += INPUT_BOOST;
        if (snap.powerslide && colorPowerslide)
            input += INPUT_POWERSLIDE;
        /*if (snap.throttle)
            input += INPUT_THROTTLE;*/
        if (snap.steer != 0 && colorSteer)
            input += INPUT_STEER;

        if (input != currentInput)
        {
            rec->segments.push_back(TurningSegment{ i, input });
        }
        currentInput = input;
    }

    Vector2 firstVec = rec->points.front();
    Vector2 lastVec = rec->points.back();

    rec->isTurningLeft = lastVec.X < firstVec.X;
}

void TurnExercise::visualize(CanvasWrapper canvas)
{
    if (!cvarManager->getCvar("turn_graph_enabled").getBoolValue())
        return;
    
    TurningRecording* recording = this->getLastRecording();

    if (recording->snapshots.size() <= 0)
        return;

    int drawingX = cvarManager->getCvar("turn_graph_x").getIntValue();
    int drawingY = cvarManager->getCvar("turn_graph_y").getIntValue();
    int drawingScale = cvarManager->getCvar("turn_graph_scale").getIntValue();

    /*canvas.SetPosition(Vector2{ drawingX, drawingY });
    canvas.SetColor(100, 100, 100, 200);
    canvas.DrawBox(Vector2{ drawingScale, drawingScale });*/

    Vector2 origin = Vector2{ drawingX, drawingY + drawingScale };
    if (recording->isTurningLeft)
        origin.X += drawingScale;

    int width = recording->pbound.maxX - recording->pbound.minX;
    int height = recording->pbound.maxY - recording->pbound.minY;
    float scale = (float)drawingScale / (float)std::max(width, height);

    Vector2 lastPoint = recording->points.front();
    Vector2 lastCoord = origin;

    bool colorBoost = cvarManager->getCvar("turn_graph_boost").getBoolValue();
    bool colorPowerslide = cvarManager->getCvar("turn_graph_powerslide").getBoolValue();
    bool colorSteer = cvarManager->getCvar("turn_graph_steer").getBoolValue();

    Vector2 legendXY = { drawingX - 200, drawingY + 30 };

    drawLegend(canvas, legendXY, colorBoost, colorPowerslide, colorSteer);

    for (int i = 1; i < recording->points.size(); i++)
    {
        Vector2 point = recording->points.at(i);
        Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };

        RGBA color = getSnapshotColor(recording->snapshots.at(i), colorBoost, colorPowerslide, colorSteer);
        canvas.SetColor(color.R, color.G, color.B, color.A);
        util::drawThiccLine(canvas, lastCoord, coord);
        lastCoord = coord;
    }

    canvas.SetPosition(Vector2{ drawingX + drawingScale / 2 - 40, drawingY + drawingScale + 20 });
    canvas.SetColor(255, 255, 255, 255);
    canvas.DrawString("Total # ticks: " + std::to_string(recording->points.size()));

    for (int i = 0; i < recording->segments.size(); i++)
    {
        TurningSegment seg = recording->segments.at(i);
        int nextIndex;
        if (i < recording->segments.size() - 1)
            nextIndex = recording->segments.at(i + 1).startIndex;
        else
            nextIndex = recording->points.size();

        int middleIndex = (seg.startIndex + nextIndex) / 2;

        RGBA color = getSnapshotColor(recording->snapshots.at(middleIndex), colorBoost, colorPowerslide, colorSteer);
        canvas.SetColor(color.R, color.G, color.B, color.A);
        Vector2 point = recording->points.at(middleIndex);
        Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };
        if (recording->isTurningLeft)
            coord.X += 20;
        else
            coord.X -= 20;

        canvas.SetPosition(coord);
        canvas.DrawString(std::to_string(nextIndex - seg.startIndex), 1.0, 1.0);
    }
}