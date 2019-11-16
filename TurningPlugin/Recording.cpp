#include "Recording.h"

void TurningRecording::reset()
{
	snapshots.clear();
	pbound = PointsBoundary{ 0, 0, 0, 0 };
	points.clear();
	segments.clear();
}

RGBA getColor(bool boost, bool powerslide, bool steer, bool showBoost, bool showPowerslide, bool showSteer)
{
	char red = (showBoost && boost) ? (char)255 : 0;
	char green = (showPowerslide && powerslide) ? (char)255 : 0;
	char blue = (showSteer && steer) ? (char)255 : 0;

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

RGBA getSnapshotColor(TurningSnapshot snap, bool showBoost, bool showPowerslide, bool showSteer)
{
	return getColor(snap.boost, snap.powerslide, snap.steer != 0, showBoost, showPowerslide, showSteer);
}


void drawLegendEntry(CanvasWrapper canvas, Vector2 pos, RGBA color, std::string text)
{
	canvas.SetColor(color.R, color.G, color.B, color.A);
	canvas.SetPosition(pos);
	canvas.FillBox(Vector2{ 10, 10 });
	canvas.SetPosition(Vector2{ pos.X + 15, pos.Y - 4 });
	canvas.DrawString(text, 1, 1);
}

std::string buildLegendEntryText(bool colorBoost, bool colorPowerslide, bool colorSteer)
{
	std::string s = "";
	s += colorBoost ? "boost + " : "";
	s += colorPowerslide ? "powerslide + " : "";
	s += colorSteer ? "steer" : "";

	if (s.length() == 0)
	{
		s = "nothing";
	}
	else if (s.substr(s.length() - 2).compare("+ ") == 0)
	{
		s = s.substr(0, s.length() - 2);
	}

	return s;
}

void drawLegend(CanvasWrapper canvas, Vector2 pos, bool colorBoost, bool colorPowerslide, bool colorSteer)
{
	int trueCount = (colorBoost ? 1 : 0) + (colorPowerslide ? 1 : 0) + (colorSteer ? 1 : 0);
	int i = 0;
	int x = pos.X;
	int y = pos.Y;

	if (trueCount == 3)
	{
		for (bool b1 : { false, true })
		{
			for (bool b2 : { false, true })
			{
				for (bool b3 : { false, true })
				{
					RGBA color = getColor(b1, b2, b3, colorBoost, colorPowerslide, colorSteer);
					drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(b1, b2, b3));
					i++;
				}
			}
		}
	}
	else if (trueCount == 2)
	{
		for (bool b1 : { false, true })
		{
			for (bool b2 : { false, true })
			{
				RGBA color;
				if (!colorBoost)
				{
					color = getColor(false, b1, b2, colorBoost, colorPowerslide, colorSteer);
					drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(false, b1, b2));
				}
				else if (!colorPowerslide)
				{
					color = getColor(b1, false, b2, colorBoost, colorPowerslide, colorSteer);
					drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(b2, false, b2));
				}
				else if (!colorSteer)
				{
					color = getColor(b1, b2, false, colorBoost, colorPowerslide, colorSteer);
					drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(b1, b2, false));
				}
				i++;
			}
		}
	}
	else if (trueCount == 1)
	{
		for (bool b1 : { false, true })
		{
			RGBA color;
			if (colorBoost)
			{
				color = getColor(b1, false, false, colorBoost, colorPowerslide, colorSteer);
				drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(b1, false, false));
			}
			else if (colorPowerslide)
			{
				color = getColor(false, b1, false, colorBoost, colorPowerslide, colorSteer);
				drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(false, b1, false));
			}
			else if (colorSteer)
			{
				color = getColor(false, false, b1, colorBoost, colorPowerslide, colorSteer);
				drawLegendEntry(canvas, { x, y + i * 20 }, color, buildLegendEntryText(false, false, b1));
			}
			i++;
		}
	}
}