#define _USE_MATH_DEFINES
#include <math.h>
#include "FreeTurnExercise.h"
#include "utils.h"

FreeTurnExercise::FreeTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
    this->game = game;
    this->cvarManager = cvarManager;

    this->recording[0] = new TurningRecording();
    this->recording[1] = new TurningRecording();
}

void FreeTurnExercise::init()
{
    game->RegisterDrawable(std::bind(&FreeTurnExercise::visualize, this, std::placeholders::_1));
    util::hookPhysicsTick(game, std::bind(&FreeTurnExercise::tick, this));
    this->recording[0]->reset();
    this->recording[1]->reset();
    reset();
}

void FreeTurnExercise::reset()
{
    this->isTurning = false;
    this->ticksWithSameRot = 0;
    this->currRecordingBuffer = 0;
    
    this->isActive = true;
}

void FreeTurnExercise::tick()
{
    if (this->isActive)
    {
        Rotator currentRot = util::getCar(game).GetRotation();

        ControllerInput input = util::getCar(game).GetInput();
        if (!isTurning && input.Steer != 0)
        {
            isTurning = true;
            this->startRot = currentRot;
            this->lastRot = currentRot;
            this->ticksWithSameRot = 0;
            this->getCurrentRecording()->reset();
        }

        if (isTurning)
        {
            saveSnapshot();
        }

        if (isTurning && input.Steer == 0) // stopped turning
        {
            if (util::isInRotRange(lastRot, currentRot, 100))
            {
                ticksWithSameRot++;

                int straightTreshold = cvarManager->getCvar("turn_free_straight_treshold").getIntValue();

                if (ticksWithSameRot > straightTreshold)
                {
                    finalRot = currentRot;
                    isTurning = false;
                    end();
                }
            }
            else
            {
                ticksWithSameRot = 0;
            }
        }

        lastRot = currentRot;
    }    
}

void FreeTurnExercise::end()
{
    swapRecordingBuffers();
    analyzeTurn(this->getLastRecording());
}

void FreeTurnExercise::clear()
{
    this->isActive = false;
    game->UnregisterDrawables();
    util::unhookPhysicsTick(game);
}