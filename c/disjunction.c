//collection definitions *before* inclusion of disjunction.h! (definitions in header lead to multiple definitions due to this: http://stackoverflow.com/questions/14425262/why-include-guards-do-not-prevent-multiple-function-definitions)

#include "disjunction.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //for vprintf -- to console
#include <windows.h>

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

//--------- Device --------------//
void Device_constructor(Device * this, int channelsLength)
{
	this->channelsLength = channelsLength;
	this->channels = malloc(channelsLength * sizeof(Device));
}

//--------- Pointer -------------//

void Pointer_updateSelected(Pointer * const this)
{

}
/*
this.setTargetLast = function()
	{
		this.targetLast = this.target;
	}
	
	this.progressDrag = function()
	{
		this.dragging.bounds.x0 += this.xChannel.delta;
		this.dragging.bounds.x1 += this.xChannel.delta;
		this.dragging.bounds.y0 += this.yChannel.delta;
		this.dragging.bounds.y1 += this.yChannel.delta;
	}
	//generally called by view to do some animation such as border highlight
	this.hasChangedTarget = function()
	{
		return this.target !== this.targetLast;
	}
	
	this.hasChangedSelected = function()
	{
		return this.isSelected != this.wasSelected;
	}
	
	this.hasMoved = function()
	{
		return this.xChannel.delta != 0 || this.yChannel.delta != 0;
	}
*/
//--------- Disjunction ---------//

double Disjunction_getDeltaSec(double counterDelta, double counterFrequency)
{
	
	//return (float)counterDelta.QuadPart /(float)frequency.QuadPart;
}

void Disjunction_update(Disjunction * const this)
{
	//printf("Disjunction_update\n");
	//printf("Disjunction_update disjunction.timer->deltaSec %f\n",  this->timer->deltaSec);
	
	for (int i = 0; i < this->apps.count; i++)
	{
		App * app = this->apps.entries[i];
		if (app->running)
			App_update(app);
	}
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

void Disjunction_initialise(Disjunction * const this)
{
	this->apps.keys = this->_appKeys;
	this->apps.entries = (void *) &this->_apps;
	this->apps.capacity = sizeof(this->_apps);
	this->apps.fail = NULL;
	
	this->devices.keys = this->_deviceKeys;
	this->devices.entries = (void *) &this->_devices;
	this->devices.capacity = sizeof(this->_devices);
	this->devices.fail = NULL;
	
	//TODO services?
	
	if (this->initialise)
		this->initialise((void *)this);
		
	this->initialised = true;
}
void Disjunction_start(Disjunction * const this)
{
	if (!this->timer->running)
		Timer_start(this->timer);
}

void Disjunction_stop(Disjunction * const this)
{
	if (this->timer->running)
		Timer_stop(this->timer);
	
	//this->stop((void *)this);
}

//dispose removes resources acquired in initialise or updates
void Disjunction_dispose(Disjunction * const this)
{
	for (int i = 0; i < this->apps.count; i++)
	{
		App * app = this->apps.entries[i];
		if (app)
			App_dispose(app);
	}
	
	if (this->dispose)
		this->dispose((void *)this);
	
	this->initialised = false;
	printf ("Disjunction_dispose done."); 
	//free(this); //disjunction object is not a pointer! it's an automatic global variable allocated on the stack.
}

void Disjunction_addApp(Disjunction * const this, const char * id, App * const app)
{
	if (app)
	{
		printf("app->id? %d\n", app->id == NULL);
		put(&this->apps, app->id, app);
		app->disjunction = this;
	}
	else
	{
		printf ("Disjunction_addApp - Error: NULL app pointer.\n"); 
		exit(1);
	}
}

//--------- App ---------//
void App_update(App * const this)
{
	//printf("App_update\n");
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	//Pointer * pointer = this->disjunction->pointer;
	/*
	if (pointer)
	{
		//if (view->enabled) //root enabled
		//{
			Pointer_findTarget(pointer, view);
			Pointer_updateSelected(pointer);
		//}
	}
	*/

	//Ctrl * ctrl = this->ctrl;
	//View * view = this->view;
	/*
	Pointer * pointer = this->disjunction->pointer;
	Pointer_updateSelected(pointer); //final
	*/
	
	//app->input uses raw & pointer input, and custom input methods on services, to:
	//-write to model AND/OR views (if we want view-specific values)
	//-do any pre-processing of raw input necessary. remember that picking may come from elsewhere e.g. framebuffer
	//-apply input abstraction as per app-specific rules
	//-custom-calculate pointer position within Views (one, some or all - as desired) - may require top-level processing due to nature of incoming info e.g. framebuffer ids
	
	//this->input(this); //abstract
	Ctrl_update(ctrl); //abstract
	//if (view != NULL) //JIC user turns off the root view by removing it (since this is the enable/disable mechanism)
		View_update(view); //final, though view->output called hereby is abstract
	//Ctrl_updatePost(ctrl); //abstract
}

void App_start(App * const this)
{
	printf("App_start\n");
	
	if (!this->running)
	{
		Ctrl * ctrl = this->ctrl;
		View * view = this->view;
		
		if (!this->initialised)
		{
			//for now, check ALL on initialisation (TODO - move to relevant sections and IFDEF DEBUG)
			if(!this->initialise)
			{
				printf ("App_start - Error: missing initialise function.\n"); 
				exit(1);
			}
			
			if(!this->dispose)
			{
				printf ("App_start - Error: missing dispose function.\n"); 
				exit(1);
			}
			
			/*
			if(!this->input)
			{
				printf ("App_start - Error: missing input function.\n"); 
				exit(1);
			}
			
			if(!this->start)
			{
				printf ("App_start - Error: missing start function.\n"); 
				exit(1);
			}
			
			if(!this->stop)
			{
				printf ("App_start - Error: missing stop function.\n"); 
				exit(1);
			}
			*/
			if(!this->initialise)
			{
				printf ("App_start - Error: missing initialise function.\n"); 
				exit(1);
			}
			
			if(!this->dispose)
			{
				printf ("App_start - Error: missing dispose function.\n"); 
				exit(1);
			}
			
			if(!this->model)
			{
				printf ("App_start - Error: missing model.\n"); 
				exit(1);
			}
			
			if(!this->view)
			{
				printf ("App_start - Error: missing view.\n"); 
				exit(1);
			}
			else
			{
				if (!this->view->start)
				{
					printf ("App_start - Error: missing view start function.\n"); 
					exit(1);
				}
				
				if (!this->view->stop)
				{
					printf ("App_start - Error: missing view stop function.\n"); 
					exit(1);
				}
				
				if (!this->view->initialise)
				{
					printf ("App_start - Error: missing view initialise function.\n"); 
					exit(1);
				}
				
				if (!this->view->dispose)
				{
					printf ("App_start - Error: missing view dispose function.\n"); 
					exit(1);
				}
				
				if (!this->view->update)
				{
					printf ("App_start - Error: missing view update function.\n"); 
					exit(1);
				}
				
				if (!this->view->updatePost)
				{
					printf ("App_start - Error: missing view updatePost function.\n"); 
					exit(1);
				}
				
				//TODO recurse view children.
			}
			
			if(!this->ctrl)
			{
				printf ("App_start - Error: missing ctrl.\n"); 
				exit(1);
			}
			else
			{
				if (!this->ctrl->mustStart)
				{
					printf ("App_start - Error: missing ctrl mustStart function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->mustStop)
				{
					printf ("App_start - Error: missing ctrl mustStop function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->start)
				{
					printf ("App_start - Error: missing ctrl start function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->stop)
				{
					printf ("App_start - Error: missing ctrl stop function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->initialise)
				{
					printf ("App_start - Error: missing ctrl initialise function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->dispose)
				{
					printf ("App_start - Error: missing ctrl dispose function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->update)
				{
					printf ("App_start - Error: missing ctrl update function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->updatePost)
				{
					printf ("App_start - Error: missing ctrl updatePost function.\n"); 
					exit(1);
				}
			}
			
			//initialise app
			this->initialise((void *)this);
		}
		
		this->running = true;
		
		ctrl->initialise((void *)ctrl); //DEV - should be in Ctrl_start()
		view->initialise((void *)view); //DEV - should be in View_start()
		ctrl->start((void *)ctrl);
		view->start((void *)view);
	}
}

void App_stop(App * const this)
{
	printf("App_stop\n");

	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	ctrl->stop((void *)ctrl);
	view->stop((void *)view);
	
	this->running = false;
}

//dispose removes resources acquired in initialise or updates
void App_dispose(App * const this)
{
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;

	Ctrl_disposeRecurse(ctrl);
	View_disposeRecurse(view);
	
	//this->services.dispose();
	//this->devices.dispose();
	
	this->dispose((void *)this);
	this->initialised = false;
	free(this);
}

void Ctrl_update(Ctrl * const this)
{
	this->update(this);//deltaSec
}

void Ctrl_updatePost(Ctrl * const this)
{
	this->updatePost(this);//deltaSec
}

void Ctrl_dispose(Ctrl * const this)
{
	this->dispose(this);
	this->initialised = false;
}

void Ctrl_disposeRecurse(Ctrl * const this)
{
	//TODO recurse
	this->dispose(this);
	this->initialised = false;
	//free(this);
}

void View_update(View * const this)
{
	//we just remove from the parent's child collections if we don't want a View to run!
	//if (this->enabled) //(this->running)
	//{
		this->update(this);//deltaSec

		List childrenByDrawOrder = this->childrenByDrawOrder;
		int length = childrenByDrawOrder.length;
		for (int i = length - 1; i > -1; i--)
		//for (int i = 0; i < length; i++)
		{
			View * child = (View *)childrenByDrawOrder.entries[i];
			View_update(child);//deltaSec //only works if enabled
			
		}
		
		this->updatePost(this);//deltaSec
	//}
}

void View_disposeRecurse(View * const this)
{
	printf("View_disposeRecurse\n");
	//TODO recurse
	this->dispose(this);
	this->initialised = false;
	//free(this);
	//_aligned_free(this);
};

void View_addChild(View * const this, View * const child)
{
	Map childrenById  = this->childrenById;
	
	if (put(&childrenById, (&child->id[0]), child)) //only do the full process if able to add to the map
	//TODO there are more things to check here... e.g. make sure that capcity of both collections is same (do in init())
	{
		child->parent = this;
		List childrenByDrawOrder = this->childrenByDrawOrder;
		add(&childrenByDrawOrder, child);
	}
}

/*
void View_removeChild(View * const this, View * const child)
{
	Map childrenById  = this->childrenById;
	
	if (put(&childrenById, child->id, child)) //only do the full process if able to add to the map
	//TODO there are more things to check here... e.g. make sure that capcity of both collections is same (do in init())
	{
		child->parent = this;
		List childrenByDrawOrder = this->childrenByDrawOrder;
		add(&childrenByDrawOrder, child);
	}
}
*/
void View_swapChildren(View * const this, int indexFrom, int indexTo)
{
}

bool View_isRoot(View * const this)
{
	return this->parent == NULL;
}

void NullFunction(void * const this){/*printf("NullFunction\n");*/}