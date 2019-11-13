#include "Recording.h"

void TurningRecording::reset()
{
	snapshots.clear();
	pbound = PointsBoundary{ 0, 0, 0, 0 };
	points.clear();
	segments.clear();
}

RGBA getSnapshotColor(TurningSnapshot snap)
{
	if (snap.boost && snap.powerslide)
		return RGBA{ (char)255, (char)255, (char)0, (char)255 };
	else if (snap.boost)
		return RGBA{ (char)255, (char)0, (char)0, (char)255 };
	else if (snap.powerslide)
		return RGBA{ (char)0, (char)255, (char)0, (char)255 };
	else
		return RGBA{ (char)255, (char)255, (char)255, (char)255 };
}