#pragma once
#include "bakkesmod/wrappers/GameWrapper.h"

struct TurningSnapshot
{
	Vector location;
	Rotator rotation;

	float throttle;
	float steer;
	bool boost;
	bool powerslide;
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

	void reset();
};

struct RGBA
{
	char R, G, B, A;
};

RGBA getSnapshotColor(TurningSnapshot snap, bool showBoost, bool showPowerslide, bool showSteer);