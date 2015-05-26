#include "feedback.h"
#include "ch.h"
#include "hal.h"


void feedback_get (struct feedback left, struct feedback right)
{
	left.position = 0;
	right.position = 0;

	left.velocity = 0;
	right.velocity = 0;

	left.current = 0;
	right.current = 0;
}