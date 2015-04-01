#ifndef TIMER_H
#define TIMER_H

#include <time.h>

typedef struct Timer
{
	float baseSec; /** internal */
	float deltaSec; /** public read / internal write */
	float accumulatorSec;
	float period;
	
	bool running;
	
	//Windows - QPC
	__int64 counter1;
	__int64 counter2;
	__int64 counterDelta;
	__int64 counterFrequency;
	double counterPeriod;
	
} Timer;

Timer * const Timer_constructor(float period);
int Timer_canConsume(Timer * const this);
void Timer_accumulate(Timer * const this);
void Timer_getDeltaSec(Timer * const this);
void Timer_consume(Timer * const this);
void Timer_start(Timer * const this);
void Timer_stop(Timer * const this);

#endif //TIMER_H