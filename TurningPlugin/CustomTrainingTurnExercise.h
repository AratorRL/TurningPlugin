#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "TurnExercise.h"
#include "Recording.h"


class CustomTrainingTurnExercise : public TurnExercise
{
private:
    Rotator startRot;
    Rotator finalRot;
    Rotator lastRot;
    int ticksWithSameRot;
    bool isTurning;
    bool hitBall;
    bool isDriving;
    bool hasJustReset;

public:
    CustomTrainingTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager);

    void init() override;
    void reset() override;
    void tick() override;
    void end() override;
    void clear() override;

    void OnHitBall();
    void OnResetRound();
};
