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

RGBA getColor(bool boost, bool powerslide, bool steer, bool showBoost, bool showPowerslide, bool showSteer);
RGBA getSnapshotColor(TurningSnapshot snap, bool showBoost, bool showPowerslide, bool showSteer);
void drawLegendEntry(CanvasWrapper canvas, Vector2 pos, RGBA color, std::string text);
std::string buildLegendEntryText(bool colorBoost, bool colorPowerslide, bool colorSteer);
void drawLegend(CanvasWrapper canvas, Vector2 pos, bool colorBoost, bool colorPowerslide, bool colorSteer);