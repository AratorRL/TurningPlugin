#include "Monitor.h"
#include "utils.h"

void Monitor::startRecordingInput()
{
    if (!isRecordingInput)
    {
        isRecordingInput = true;

        logger->log("monitor: hooking physics tick");
        util::hookPhysicsTick(game, std::bind(&Monitor::saveInputSnapshot, this));
    }
}

void Monitor::stopRecordingInput()
{
    logger->log("monitor: stopping recording");
    
    isRecordingInput = false;
    util::unhookPhysicsTick(game);
    game->RegisterDrawable(std::bind(&Monitor::visualize, this, std::placeholders::_1), LAYER_MONITOR);
}

void Monitor::saveInputSnapshot()
{
    // logger->log("saving input snapshot");
    
    CarWrapper car = util::getCar(game);
    Vector loc = car.GetLocation();
    Rotator rot = car.GetRotation();

    ControllerInput input = util::getCar(game).GetInput();
    
    this->recording.snapshots.push_back({
        loc,
        rot,
        input.Throttle,
        input.Steer,
        (bool)input.HoldingBoost,
        (bool)input.Handbrake
    });

    if (loc.X < recording.minX)
        recording.minX = loc.X;
    if (loc.X > recording.maxX)
        recording.maxX = loc.X;
    if (loc.Y < recording.minY)
        recording.minY = loc.Y;
    if (loc.Y > recording.maxY)
        recording.maxY = loc.Y;

    if (config->isFinished())
    {
        stopRecordingInput();
    }
}

Vector2 Monitor::getLocalCoordinate(TurningSnapshot snap, TurningRecording recording, float scale, Vector2 center)
{
    int width = (recording.maxX - recording.minX);
    int height = (recording.maxY - recording.minY);

    logger->log("width, height:");
    logger->logi(width);
    logger->logi(height);

    int x = (snap.location.X - recording.minX);
    int y = (snap.location.Y - recording.minY);

    logger->logi(x);
    logger->logi(y);

    return Vector2{ center.X + x, center.Y + y };
}

void Monitor::visualize(CanvasWrapper canvas)
{
    logger->log("monitor: visualizing");
    
    if (recording.snapshots.size() <= 0)
        return;

    logger->log("# snapshots found:");
    logger->logi(recording.snapshots.size());    

    canvas.SetPosition(Vector2 { 300, 300 });
    canvas.FillBox(Vector2 { 300, 300 });

    Vector2 center = { 1280 / 2, 720 / 2 };
    
    float scale = 1;

    TurningSnapshot snap0 = recording.snapshots.at(0);
    Vector2 lastCoord = getLocalCoordinate(snap0, recording, scale, center);

    for (int i = 1; i < recording.snapshots.size(); i++)
    {
        TurningSnapshot snap = recording.snapshots.at(i);
        Vector2 coord = getLocalCoordinate(snap, recording, scale, center);

        canvas.DrawLine(lastCoord, coord);
        lastCoord = coord;
    }
}