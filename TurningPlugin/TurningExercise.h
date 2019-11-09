#pragma once

#define INPUT_NONE 0
#define INPUT_BOOST 1
#define INPUT_POWERSLIDE 2
#define INPUT_THROTTLE 4

class TurningExercise
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