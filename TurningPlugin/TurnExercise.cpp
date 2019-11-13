#include "TurnExercise.h"
#include "utils.h"

TurningRecording* TurnExercise::getCurrentRecording()
{
	return this->recording[currRecordingBuffer];
}

TurningRecording* TurnExercise::getLastRecording()
{
	return this->recording[1 - currRecordingBuffer];
}

void TurnExercise::swapRecordingBuffers()
{
	this->currRecordingBuffer = 1 - this->currRecordingBuffer;
}

void TurnExercise::saveSnapshot()
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