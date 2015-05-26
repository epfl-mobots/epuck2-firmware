#include "ch.h"
#include "hal.h"
#include "setpoints.h"

void setpoints_get(struct setpoints left, struct setpoints right)
{
	left.position = 0;
	right.position = 0;

	left.velocity = 0;
	right.velocity = 0;

	left.current = 0;
	right.current = 0;
}