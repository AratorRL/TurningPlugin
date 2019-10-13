#pragma once
#include <vector>

#include "Logger.h"
#include "FreeTurnConfiguration.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/wrapperstructs.h"

struct TurningSnapshot
{
    Vector location;
    Rotator rotation;

    float throttle;
    float steer;
    bool boost;
    long powerslide;
};

struct TurningRecording
{
    std::vector<TurningSnapshot> snapshots;
    int minX = 100000;
    int maxX = -100000;
    int minY = 100000;
    int maxY = -100000;
};

class Monitor
{
private:
    std::shared_ptr<GameWrapper> game;
    Logger* logger;
    FreeTurnConfiguration* config;

    TurningRecording recording;
    bool isRecordingInput = false;

    void saveInputSnapshot();
    Vector2 getLocalCoordinate(TurningSnapshot snap, TurningRecording recording, float scale, Vector2 center);

public:
    Monitor(std::shared_ptr<GameWrapper> game, Logger* logger, FreeTurnConfiguration* config) : game(game), logger(logger), config(config) {};

    void startRecordingInput();
    void stopRecordingInput();

    void visualize(CanvasWrapper canvas);
};