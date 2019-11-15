#pragma once
#include "bakkesmod/wrappers/GameWrapper.h"

namespace util {
    CarWrapper getCar(std::shared_ptr<GameWrapper> game);
    Rotator getCarRotation(std::shared_ptr<GameWrapper> game);

    BallWrapper getBall(std::shared_ptr<GameWrapper> game);
    Vector getBallLocation(std::shared_ptr<GameWrapper> game);

    Rotator turnClockwise(Rotator rot, int angle);

    string vecToString(Vector v);
    string rotToString(Rotator r);
    bool isInYawRange(int currYaw, int goalYaw, int range);
    bool isInRotRange(Rotator currRot, Rotator goalRot, int range);
    bool isInRange(Vector relativeLoc, Rotator targetRot, int range);
	Vector2 rotateVec2(Vector2F vec, float angle);

	void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);

    void hookPhysicsTick(std::shared_ptr<GameWrapper> game, std::function<void(std::string eventName)> callback);
    void unhookPhysicsTick(std::shared_ptr<GameWrapper> game);
}