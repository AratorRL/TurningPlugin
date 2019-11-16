#define _USE_MATH_DEFINES
#include <math.h>
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
	
	// make the difference is < 65536
	while (abs(currYaw - goalYaw) >= 32768)
	{
		if (currYaw < goalYaw)
			currYaw += 65536;
		else
			goalYaw += 65536;
	}
    return (currYaw >= goalYaw - range) && (currYaw <= goalYaw + range);
}

bool util::isInRotRange(Rotator currRot, Rotator goalRot, int range)
{
    int currYaw = currRot.Yaw;
    int goalYaw = goalRot.Yaw;

    return isInYawRange(currYaw, goalYaw, range);
}

bool util::isInRange(Vector relativeLoc, Rotator targetRot, int range)
{
	float angle = targetRot.Yaw * M_PI / 32768 - M_PI / 2;
}

Vector2 util::rotateVec2(Vector2F vec, float angle)
{
	int x = (int)((float)vec.X * cos(angle) - (float)vec.Y * sin(angle));
	int y = (int)((float)vec.X * sin(angle) + (float)vec.Y * cos(angle));
	return Vector2{ x, y };
}

void util::drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			Vector2 s = { start.X + i, start.Y + j };
			Vector2 e = { end.X + i, end.Y + j };
			cw.DrawLine(s, e);
		}
	}
}

void util::hookPhysicsTick(std::shared_ptr<GameWrapper> game, std::function<void(std::string eventName)> callback)
{
    game->HookEvent("Function TAGame.Car_TA.SetVehicleInput", callback);
}

void util::unhookPhysicsTick(std::shared_ptr<GameWrapper> game)
{
    game->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
}