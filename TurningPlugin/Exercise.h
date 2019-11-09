#pragma once

#define INPUT_NONE 0
#define INPUT_BOOST 1
#define INPUT_POWERSLIDE 2
#define INPUT_THROTTLE 4

enum ExerciseType {
	FreeTurn = 0,
	FixedStart = 1
};

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

	void reset()
	{
		snapshots.clear();
		pbound = PointsBoundary{ 0, 0, 0, 0 };
		points.clear();
		segments.clear();
	}
};

class Exercise
{
public:
	// whether the exercise is still active
	bool isActive;

	// setup valid starting state, if needed
	virtual void init() = 0;

	// reset the exercise
	virtual void reset() = 0;

	// gets called every physics tick while exercise is active
	virtual void tick() = 0;

	// end the exercise
	virtual void end() = 0;

	virtual void clear() = 0;
};