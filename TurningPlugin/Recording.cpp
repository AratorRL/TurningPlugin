#include "Recording.h"

void TurningRecording::reset()
{
	snapshots.clear();
	pbound = PointsBoundary{ 0, 0, 0, 0 };
	points.clear();
	segments.clear();
}

RGBA getSnapshotColor(TurningSnapshot snap, bool showBoost, bool showPowerslide, bool showSteer)
{
	char red = (showBoost && snap.boost) ? (char)255 : 0;
	char green = (showPowerslide && snap.powerslide) ? (char)255 : 0;
	char blue = (showSteer && snap.steer != 0) ? (char)255 : 0;

	if (red == 0 && green == 0 && blue == 0)
	{
		red = 255;
		green = 255;
		blue = 255;
	}
	else if (showBoost && (red == (char)255) && showPowerslide && (green == (char)255) && showSteer && (blue == (char)255))
	{
		red = 0;
		green = 0;
		blue = 0;
	}

	return RGBA{ red, green, blue, (char)255 };
}