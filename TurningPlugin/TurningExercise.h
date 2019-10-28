#pragma once

#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

#include "Exercise.h"

#define INPUT_NONE 0
#define INPUT_BOOST 1
#define INPUT_POWERSLIDE 2
#define INPUT_THROTTLE 4

struct TurningSnapshot
{
	Vector location;
	Rotator rotation;

	float throttle;
	float steer;
	bool boost;
	long powerslide;
};

struct TurningSegment
{
	int startIndex;
	int inputType;
};

struct PointsBoundary {
	int minX;
	int maxX;
	int minY;
	int maxY;
};

class TurningRecording
{
public:
	std::vector<TurningSnapshot> snapshots;
	std::vector<Vector2> points;
	PointsBoundary pbound;
	std::vector<TurningSegment> segments;
	bool isTurningLeft;

	void analyze();

	void reset()
	{
		snapshots.clear();
		pbound = PointsBoundary{ 0, 0, 0, 0 };		
		points.clear();
		segments.clear();
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

	void drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end);
	void visualize(CanvasWrapper canvas);

	TurningRecording* getCurrentRecording();
	TurningRecording* getLastRecording();
};