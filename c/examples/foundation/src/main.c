#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //for vprintf -- to console

#include "../../../core/Disjunction.h"

int c = 0;

typedef struct BasicView
{
	View base;
	int y;
} BasicView;

typedef struct BasicCtrl
{
	Ctrl base;
	int x;
} BasicCtrl;

int main(int argc, char *argv[])
{
	Disjunction disjunction;
	disjunction.timer = Timer_constructor(0.016666667);
	disjunction.timer->disjunction = &disjunction;
	
	//TODO parser, create apps
	App app;
	app.disjunction = &disjunction;
	disjunction.apps[0] = &app; //more likely, we would pass pointer to existing element at 0 (and so forth) to Builder
	
	printf("boo %i", 13);
	
	Disjunction_start((Disjunction * const)&disjunction);
	Timer * timer = disjunction.timer;
	
	while (1)
	{
		Timer_updateDeltaSec(timer);
		printf("deltaSec %.10f\n", timer->deltaSec);
		
		Timer_accumulate(timer);
		while (Timer_canUpdate(timer))
		{
			Timer_update(timer);
			c++;
		}
		
		printf("c=%i\n", c);
		c = 0;
	}
	
}


