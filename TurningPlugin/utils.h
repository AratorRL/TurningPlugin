#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"

namespace util {
    CarWrapper getCar(std::shared_ptr<GameWrapper> game);
    Rotator getCarRotation(std::shared_ptr<GameWrapper> game);
    string vecToString(Vector v);
    string rotToString(Rotator r);
    bool isInYawRange(int currYaw, int goalYaw, int range);
    bool isInRotRange(Rotator currRot, Rotator goalRot, int range);
}