#pragma once

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
};