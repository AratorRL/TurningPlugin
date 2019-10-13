#include "utils.h"

CarWrapper util::getCar(std::shared_ptr<GameWrapper> game)
{
    ServerWrapper server = game->GetGameEventAsServer();
    return server.GetGameCar();
}

Rotator util::getCarRotation(std::shared_ptr<GameWrapper> game)
{
    return getCar(game).GetRotation();
}

BallWrapper util::getBall(std::shared_ptr<GameWrapper> game)
{
    ServerWrapper server = game->GetGameEventAsServer();
    return server.GetBall();
}

Vector util::getBallLocation(std::shared_ptr<GameWrapper> game)
{
    return getBall(game).GetLocation();
}

string util::vecToString(Vector v)
{
    return "(" + to_string(v.X) + ", " + to_string(v.Y) + ", " + to_string(v.Z) + ")";
}

string util::rotToString(Rotator r)
{
    return "(" + to_string(r.Pitch) + ", " + to_string(r.Yaw) + ", " + to_string(r.Roll) + ")";
}

Rotator util::turnClockwise(Rotator rot, int angle)
{
    int newYaw = rot.Yaw - angle;
    if (newYaw < -32768)
        newYaw += 65536;

    return Rotator{ rot.Pitch, newYaw, rot.Roll };
}

bool util::isInYawRange(int currYaw, int goalYaw, int range)
{
    // yaw range: -32768 to +32767

    currYaw += 100000;
    goalYaw += 100000;

    return (currYaw >= goalYaw - range) && (currYaw <= goalYaw + range);
}

bool util::isInRotRange(Rotator currRot, Rotator goalRot, int range)
{
    int currYaw = currRot.Yaw;
    int goalYaw = goalRot.Yaw;

    return isInYawRange(currYaw, goalYaw, range);
}

void util::hookPhysicsTick(std::shared_ptr<GameWrapper> game, std::function<void(std::string eventName)> callback)
{
    game->HookEvent("Function TAGame.Car_TA.SetVehicleInput", callback);
}

void util::unhookPhysicsTick(std::shared_ptr<GameWrapper> game)
{
    game->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
}