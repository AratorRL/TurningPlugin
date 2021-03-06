#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "TurnExercise.h"
#include "Recording.h"


class FixedTurnExercise : public TurnExercise
{
private:
    Rotator startRot;
    Rotator finalRot;
    Rotator targetRot;
    int targetMargin;
    Rotator lastRot;
    bool isTurning;
    bool hitBall;
    bool isDriving;
    bool hasJustReset;

public:
    FixedTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager);

    void init() override;
    void reset() override;
    void tick() override;
    void end() override;
    void clear() override;

    void visualize(CanvasWrapper canvas) override;

    void OnHitBall(CarWrapper caller, void* params, std::string eventName);
    void freezeAll();
};

