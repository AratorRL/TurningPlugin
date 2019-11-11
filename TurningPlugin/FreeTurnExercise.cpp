#define _USE_MATH_DEFINES
#include <math.h>
#include "FreeTurnExercise.h"
#include "utils.h"

FreeTurnExercise::FreeTurnExercise(std::shared_ptr<GameWrapper> game, std::shared_ptr<CVarManagerWrapper> cvarManager)
{
	this->game = game;
	this->cvarManager = cvarManager;

	this->recording[0] = new TurningRecording();
	this->recording[1] = new TurningRecording();
}

void FreeTurnExercise::init()
{
	cvarManager->log("Turning exercise init.");
	game->RegisterDrawable(std::bind(&FreeTurnExercise::visualize, this, std::placeholders::_1));
	util::hookPhysicsTick(game, std::bind(&FreeTurnExercise::tick, this));
	this->recording[0]->reset();
	this->recording[1]->reset();
	reset();
}

void FreeTurnExercise::reset()
{
	this->isTurning = false;
	this->ticksWithSameRot = 0;
	this->currRecordingBuffer = 0;
	
	this->isActive = true;
}

TurningRecording* FreeTurnExercise::getCurrentRecording()
{
	return this->recording[currRecordingBuffer];
}

TurningRecording* FreeTurnExercise::getLastRecording()
{
	return this->recording[1 - currRecordingBuffer];
}

void FreeTurnExercise::swapRecordingBuffers()
{
	this->currRecordingBuffer = 1 - this->currRecordingBuffer;
}

void FreeTurnExercise::tick()
{
	if (this->isActive)
	{
		Rotator currentRot = util::getCarRotation(game);

		ControllerInput input = util::getCar(game).GetInput();
		if (!isTurning && input.Steer != 0)
		{
			isTurning = true;
			this->startRot = currentRot;
			this->lastRot = currentRot;
			this->goalRot = util::turnClockwise(startRot, 16384);
			this->ticksWithSameRot = 0;
			this->getCurrentRecording()->reset();
			cvarManager->log("Start turning.");
		}

		if (isTurning)
		{
			saveSnapshot();
		}

		if (isTurning && input.Steer == 0) // stopped turning
		{
			if (util::isInRotRange(lastRot, currentRot, 100))
			{
				ticksWithSameRot++;

				if (ticksWithSameRot > 30)
				{
					finalRot = currentRot;
					isTurning = false;
					end();
				}
			}
			else
			{
				ticksWithSameRot = 0;
			}
		}

		lastRot = currentRot;
	}	
}

void FreeTurnExercise::end()
{
	cvarManager->log("Turning exercise end.");	

	if (util::isInRotRange(finalRot, goalRot, 2000))
	{
		cvarManager->log("Goal reached.");
	}

	swapRecordingBuffers();
	analyzeTurn(this->getLastRecording());
}

void FreeTurnExercise::clear()
{
	this->isActive = false;
	game->UnregisterDrawables();
	util::unhookPhysicsTick(game);
}

void FreeTurnExercise::saveSnapshot()
{
	// cvarManager->log("saving input snapshot");

	CarWrapper car = util::getCar(game);
	Vector loc = car.GetLocation();
	Rotator rot = car.GetRotation();

	ControllerInput input = util::getCar(game).GetInput();

	TurningRecording* recording = this->getCurrentRecording();

	recording->snapshots.push_back({
		loc,
		rot,
		input.Throttle,
		input.Steer,
		(bool)input.HoldingBoost,
		(bool)input.Handbrake
	});
}

void FreeTurnExercise::analyzeTurn(TurningRecording* rec)
{
	TurningSnapshot firstSnap = rec->snapshots.front();	
	float startAngle = -firstSnap.rotation.Yaw * M_PI / 32768 - M_PI / 2;

	int currentInput = INPUT_NONE;

	for (int i = 0; i < rec->snapshots.size(); i++)
	{
		TurningSnapshot snap = rec->snapshots.at(i);
		Vector relativeLoc = snap.location - firstSnap.location;
		Vector2 vec = util::rotateVec2(Vector2F{ relativeLoc.X, relativeLoc.Y }, startAngle);
		rec->points.push_back(vec);

		if (vec.X < rec->pbound.minX)
			rec->pbound.minX = vec.X;
		if (vec.X > rec->pbound.maxX)
			rec->pbound.maxX = vec.X;
		if (vec.Y < rec->pbound.minY)
			rec->pbound.minY = vec.Y;
		if (vec.Y > rec->pbound.maxY)
			rec->pbound.maxY = vec.Y;

		int input = INPUT_NONE;
		if (snap.boost)
			input += INPUT_BOOST;
		if (snap.powerslide)
			input += INPUT_POWERSLIDE;
		if (snap.throttle)
			input += INPUT_THROTTLE;

		if (input != currentInput)
		{
			rec->segments.push_back(TurningSegment{ i, input });
		}
		currentInput = input;
	}

	Vector2 firstVec = rec->points.front();
	Vector2 lastVec = rec->points.back();

	rec->isTurningLeft = lastVec.X < firstVec.X;
}

void FreeTurnExercise::drawThiccLine(CanvasWrapper cw, Vector2 start, Vector2 end)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			Vector2 s = { start.X + i, start.Y + j };
			Vector2 e = { end.X + i, end.Y + j };
			cw.DrawLine(s, e);
		}
	}
}

LinearColor FreeTurnExercise::getColor(TurningSnapshot snap)
{
	if (snap.boost && snap.powerslide)
		return LinearColor{ 255, 255, 0, 255 };
	else if (snap.boost)
		return LinearColor{ 255, 0, 0, 255 };
	else if (snap.powerslide)
		return LinearColor{ 0, 255, 0, 255 };
	else
		return LinearColor{ 255, 255, 255, 255 };
}

void FreeTurnExercise::visualize(CanvasWrapper canvas)
{
	TurningRecording* recording = this->getLastRecording();
	
	if (recording->snapshots.size() <= 0)
		return;

	canvas.SetPosition(Vector2{ 100, 100 });
	canvas.SetColor(255, 0, 0, 255);
	// canvas.DrawString("# segments: " + to_string(recording->segments.size()));

	canvas.SetPosition(Vector2{ drawingX, drawingY });
	canvas.SetColor(100, 100, 100, 100);
	canvas.DrawBox(Vector2{ drawingWidth, drawingHeight });

	Vector2 origin = Vector2{ drawingX, drawingY + drawingHeight };
	if (recording->isTurningLeft)
		origin.X += drawingWidth;

	int width = recording->pbound.maxX - recording->pbound.minX;
	int height = recording->pbound.maxY - recording->pbound.minY;
	float scale = (float)max(drawingWidth,drawingHeight) / (float)max(width, height);

	Vector2 lastPoint = recording->points.front();
	Vector2 lastCoord = origin;

	for (int i = 1; i < recording->points.size(); i++)
	{
		Vector2 point = recording->points.at(i);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };

		LinearColor color = getColor(recording->snapshots.at(i));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		drawThiccLine(canvas, lastCoord, coord);
		lastCoord = coord;
	}

	for (int i = 0; i < recording->segments.size(); i++)
	{
		TurningSegment seg = recording->segments.at(i);
		int nextIndex;
		if (i < recording->segments.size() - 1)
			nextIndex = recording->segments.at(i + 1).startIndex;
		else
			nextIndex = recording->points.size() - 1;

		int middleIndex = (seg.startIndex + nextIndex) / 2;

		LinearColor color = getColor(recording->snapshots.at(middleIndex));
		canvas.SetColor(color.R, color.G, color.B, color.A);
		Vector2 point = recording->points.at(middleIndex);
		Vector2 coord = Vector2{ origin.X + (int)((float)point.X * scale), origin.Y + (int)((float)point.Y * scale) };
		if (recording->isTurningLeft)
			coord.X += 20;
		else
			coord.X -= 20;

		canvas.SetPosition(coord);
		canvas.DrawString(to_string(nextIndex - seg.startIndex));
	}
}