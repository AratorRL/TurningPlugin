#include "Configuration.h"

void Configuration::hookPhysicsTick()
{
    game->HookEvent("Function TAGame.Car_TA.SetVehicleInput", std::bind(&Configuration::tick, this));
}

void Configuration::unhookPhysicsTick()
{
    game->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
}

void Configuration::setAcceptingState(Rotator rot)
{
    this->acceptingState = rot;
}
