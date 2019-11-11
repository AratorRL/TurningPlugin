#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "TurningExercise.h"
#include "Recording.h"


class FixedTurnExercise : public TurningExercise
{
private:
	std::shared_ptr<GameWrapper> game;
	std::shared_ptr<CVarManagerWrapper> cvarManager;

	Rotator startRot;
	Rotator finalRot;
	Rotator goalRot;
	int goalRange;
	Rotator lastRot;
	int ticksWithSameRot;
	bool isTurning;
	bool hitBall;
	bool isDriving;
	bool hasJustReset;

	TurningRecording* recording[2];
	int currRecordingBuffer;
	int drawingX = 300;
	int drawingY = 380;
	int drawingWidth = 200;
	int drawingHeight = 200;

	void saveSnapshot();

public:
	FixedTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager);

	void init() override;
	void reset() override;
	void tick() override;
	void end() override;
	void clear() override;

	void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);
	void visualize(CanvasWrapper canvas);

	TurningRecording* getCurrentRecording();
	TurningRecording* getLastRecording();
	void swapRecordingBuffers();

	void analyzeTurn(TurningRecording* rec);
	RGBA getColor(TurningSnapshot snap);

	void OnHitBall(CarWrapper caller, void* params, std::string eventName);
	void freezeAll();
};

