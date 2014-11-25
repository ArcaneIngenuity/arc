#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //for vprintf -- to console
#include <windows.h>

#include "Disjunction.h"

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

void Timer_updateDeltaSec(Timer * const this)
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

int Timer_canUpdate(Timer * const this)
{
	return this->period <= this->accumulatorSec;
}

void Timer_update(Timer * const this)
{
	//printf("update: accumulatorSec was %.10f\n", this->accumulatorSec); 
	this->accumulatorSec -= this->period; //consume
	//printf("update: accumulatorSec is  %.10f\n", this->accumulatorSec); 
	
	Disjunction_update(this->disjunction);
}

void Timer_start(Timer * const this)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&(this->counterFrequency)); //calculate frequency just once, guaranteed to stay same thereafter.
	QueryPerformanceCounter((LARGE_INTEGER *)&(this->counter1)); //ensures we don't have a negative delta to start
	printf("QPCTimer_start %i\n", this->counter1);
	
	this->counterPeriod = 1.0 / (double)this->counterFrequency;
	printf("f/t is %.10f / %.10f\n", (double)this->counterFrequency, this->counterPeriod);
}

//--------- Device --------------//
void Device_constructor(Device * this, int channelsLength)
{
	this->channelsLength = channelsLength;
	this->channels = malloc(channelsLength * sizeof(Device));
}

//--------- Pointer -------------//

void Pointer_findTarget(Pointer * const this, void * const view)
{
}

void Pointer_updateSelected(Pointer * const this)
{
}

//--------- Disjunction ---------//

double Disjunction_getDeltaSec(double counterDelta, double counterFrequency)
{
	
	//return (float)counterDelta.QuadPart /(float)frequency.QuadPart;
}

void Disjunction_update(Disjunction * const this)
{
	//printf("Disjunction_update");

	App_update(this->apps[0]);
/*
		this.devices.poll();
		for (var id in this.apps)
		{	
			var app = this.apps[id];
			app.update();
		}
		
		this.devices.flush();
		
		for (var i = 0; i < this.services.array.length; i++)
		{
			var service = this.services.array[i];
			service.updateJournals();
		}
*/
}

void Disjunction_start(Disjunction * const this)
{
	//this->timer->disjunction = this;
	Timer_start(this->timer);
}

//--------- App ---------//
void App_update(App * const this)
{
	//printf("App_update\n");
	/*
	View * view = this->view;
	Pointer * pointer = this->disjunction->pointer;
	if (pointer)
	{
		//if (view->enabled) //root enabled
		//{
			Pointer_findTarget(pointer, view);
			Pointer_updateSelected(pointer);
		//}
	}
	*/
}

void App_dispose(App * const this)
{
	//this->ctrl.stop(); //TODO if not already stopped
	//this->services.dispose();
	//this->view.dispose();
}

//-------- Model --------//
void Model_progressJournals(void * model)//(const Journal (*journals)[JOURNALS_SIZE])
{
/*
	int length = sizeof (model->journals); //Models must always have journals[]
	
	for (int i = 0; i < length; i++)
	{
		Journal journal = model->journals[i];
		Journal_shiftEntries(); //allows combining of state and delta shift loops for fewer conditionals
		Journal_unlock();
	}
	
#define JOURNALS_SIZE 100
Journal journals[JOURNALS_SIZE];
	*/
}
