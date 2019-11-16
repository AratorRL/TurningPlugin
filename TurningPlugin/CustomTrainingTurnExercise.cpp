#include "CustomTrainingTurnExercise.h"
#include "utils.h"

CustomTrainingTurnExercise::CustomTrainingTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
    this->game = game;
    this->cvarManager = cvarManager;

    this->recording[0] = new TurningRecording();
    this->recording[1] = new TurningRecording();
}

void CustomTrainingTurnExercise::init()
{
    game->RegisterDrawable(std::bind(&CustomTrainingTurnExercise::visualize, this, std::placeholders::_1));
    util::hookPhysicsTick(game, std::bind(&CustomTrainingTurnExercise::tick, this));
    game->HookEvent("Function TAGame.Car_TA.ApplyBallImpactForces", std::bind(&CustomTrainingTurnExercise::OnHitBall, this));
    game->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartNewRound", std::bind(&CustomTrainingTurnExercise::OnResetRound, this));
    this->recording[0]->reset();
    this->recording[1]->reset();
    reset();
}

void CustomTrainingTurnExercise::OnResetRound()
{
    this->reset();
}

void CustomTrainingTurnExercise::reset()
{    
    this->currRecordingBuffer = 0;

    this->isActive = true;
    this->getCurrentRecording()->reset();
    this->hitBall = false;
    this->isDriving = false;
}

void CustomTrainingTurnExercise::OnHitBall()
{
    if (this->isActive)
    {
        CarWrapper car = util::getCar(game);
        Rotator finalRot = car.GetRotation();

        this->hitBall = true;
        this->isActive = false;
        end();
    }
}

void CustomTrainingTurnExercise::tick()
{
    if (this->isActive)
    {
        ControllerInput input = util::getCar(game).GetInput();
        this->isDriving = input.Throttle != 0.0;

        if (this->isDriving && !this->hitBall)
        {
            saveSnapshot();
        }
    }
}

void CustomTrainingTurnExercise::end()
{
    swapRecordingBuffers();
    analyzeTurn(this->getLastRecording());
}

void CustomTrainingTurnExercise::clear()
{
    this->isActive = false;
    game->UnregisterDrawables();
    util::unhookPhysicsTick(game);
    game->UnhookEvent("Function TAGame.Car_TA.ApplyBallImpactForces");
    game->UnhookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartNewRound");
}
