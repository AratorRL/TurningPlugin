#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "Exercise.h"

class FreeTurnExercise : public Exercise
{
private:
	std::shared_ptr<GameWrapper> game;
	std::shared_ptr<CVarManagerWrapper> cvarManager;

	Rotator startRot;
	Rotator finalRot;
	Rotator goalRot;
	Rotator lastRot;
	int ticksWithSameRot;
	bool isTurning;

	TurningRecording* recording[2];
	int currRecordingBuffer;
	int drawingX = 300;
	int drawingY = 380;
	int drawingWidth = 200;
	int drawingHeight = 200;

	ExerciseType type;

	void saveSnapshot();

public:
	FreeTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager, ExerciseType type);

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
	LinearColor getColor(TurningSnapshot snap);
};