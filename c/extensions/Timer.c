#include "Timer.h"

//--------- Timer ---------------//
Timer * const Timer_constructor(float period)
{
	Timer * timer = malloc(sizeof(Timer));
	
	//Timer
	timer->period = period;
	timer->accumulatorSec = 0;
	timer->deltaSec = 0;
	
	//QPC
	timer->counterPeriod = 0;
	
	return timer;
}

void Timer_getDeltaSec(Timer * const this)
{
	this->counter2 = this->counter1;
	
	QueryPerformanceCounter((LARGE_INTEGER *)&(this->counter1));
	
	this->counterDelta = this->counter1 - this->counter2;
	
	this->deltaSec = (this->counterDelta * 1.0) / (double) this->counterFrequency;
}

void Timer_accumulate(Timer * const this)
{
	this->accumulatorSec += this->deltaSec;
}

int Timer_canConsume(Timer * const this)
{
	return this->running &&
		(this->accumulatorSec >= this->period);
}

void Timer_consume(Timer * const this)
{
	//printf("update: accumulatorSec was %.10f\n", this->accumulatorSec); 
	this->accumulatorSec -= this->period; //consume
	//printf("update: accumulatorSec is  %.10f\n", this->accumulatorSec); 
}

void Timer_start(Timer * const this)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&(this->counterFrequency)); //calculate frequency just once, guaranteed to stay same thereafter.
	QueryPerformanceCounter((LARGE_INTEGER *)&(this->counter1)); //ensures we don't have a negative delta to start
	printf("QPCTimer_start %i\n", this->counter1);
	
	this->counterPeriod = 1.0 / (double)this->counterFrequency;
	printf("f/t is %.10f / %.10f\n", (double)this->counterFrequency, this->counterPeriod);
	
	this->running = true;
}

void Timer_stop(Timer * const this)
{
	//TODO !!
	QueryPerformanceFrequency((LARGE_INTEGER *)&(this->counterFrequency)); //calculate frequency just once, guaranteed to stay same thereafter.
	QueryPerformanceCounter((LARGE_INTEGER *)&(this->counter1)); //ensures we don't have a negative delta to start
	//printf("QPCTimer_start %i\n", this->counter1);
	
	this->counterPeriod = 1.0 / (double)this->counterFrequency;
	//printf("f/t is %.10f / %.10f\n", (double)this->counterFrequency, this->counterPeriod);
	
	this->running = false;
}