#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "TurnExercise.h"
#include "Recording.h"

class FreeTurnExercise : public TurnExercise
{
private:
	Rotator startRot;
	Rotator finalRot;
	Rotator goalRot;
	Rotator lastRot;
	int ticksWithSameRot;
	bool isTurning;


public:
	FreeTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager);

	void init() override;
	void reset() override;
	void tick() override;
	void end() override;
	void clear() override;
};