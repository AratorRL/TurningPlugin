#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "Exercise.h"

struct TurningSnapshot
{
	Vector location;
	Rotator rotation;

	float throttle;
	float steer;
	bool boost;
	long powerslide;
};

class TurningRecording
{
public:
	std::vector<TurningSnapshot> snapshots;
	int minX = 100000;
	int maxX = -100000;
	int minY = 100000;
	int maxY = -100000;

	void reset()
	{
		snapshots.clear();
		minX = 100000;
		maxX = -100000;
		minY = 100000;
		maxY = -100000;
	}
};

class TurningExercise : public Exercise
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

	void saveSnapshot();

public:
	TurningExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager);

	void init() override;
	void start() override;
	void tick() override;
	void end() override;

	Vector2 getLocalCoordinate(TurningSnapshot snap, TurningRecording* recording);
	void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);
	void visualize(CanvasWrapper canvas);

	TurningRecording* getCurrentRecording();
	TurningRecording* getLastRecording();
};