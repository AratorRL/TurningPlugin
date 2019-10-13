#include "FreeTurnConfiguration.h"
#include "utils.h"

void FreeTurnConfiguration::init()
{
    logger->log("initializing free turn config");

    if (!game->IsInFreeplay())
        return;

    this->startRot = util::getCarRotation(game);
    this->acceptingState = util::turnClockwise(startRot, 16384);
    
    // hookPhysicsTick();
}

bool FreeTurnConfiguration::isFinished()
{
    Rotator currentRot = util::getCarRotation(game);

    return util::isInRotRange(currentRot, acceptingState, 2000);
}

void FreeTurnConfiguration::tick()
{
    Rotator currentRot = util::getCarRotation(game);

    if (util::isInRotRange(currentRot, acceptingState, 2000))
    {
        //logger->log("ACCEPT");
        end();
    }
    else
    {
        //logger->log("NO ACCEPT");
    }

}

void FreeTurnConfiguration::end()
{
    // unhookPhysicsTick();

    logger->log("end");
}