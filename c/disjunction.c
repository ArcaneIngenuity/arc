//collection definitions *before* inclusion of disjunction.h! (definitions in header lead to multiple definitions due to this: http://stackoverflow.com/questions/14425262/why-include-guards-do-not-prevent-multiple-function-definitions)

#include "disjunction.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> //for vprintf -- to console

//#define DISJUNCTION_DEBUG

//--------- Device --------------//
void Device_constructor(Device * this, int channelsLength)
{
	this->channelsLength = channelsLength;
	this->channels = malloc(channelsLength * sizeof(Device));
}

void Device_poll(Device * this)
{
	//this->poll(this);//deltaSec
}

//--------- Pointer -------------//

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
	#ifdef DISJUNCTION_DEBUG
	printf("Disjunction_update\n");
	#endif
	
	//poll devices
	for (int i = 0; i < this->devices.count; i++)
	{
		Device * device = this->devices.entries[i]; //read from map of values
		Device_poll(device);
	}

	//update apps
	for (int i = 0; i < this->apps.count; i++)
	{
		App * app = this->apps.entries[i]; //read from map of values
		if (app->running)
			App_update(app);
	}
/*
	//flush devices
	var numDevices = this.array.length;
	for (var i = 0; i < numDevices; i++)
	{
		var device = this.array[i];
		if (device.eventBased)
		{
			var numChannels = device.channels.length;
			for (var j = 0; j < numChannels; j++)
			{
				var channel = device.channels[j];
				channel.delta = 0;
			}
		}
	}
	*/
/*
		for (var i = 0; i < this.services.array.length; i++)
		{
			var service = this.services.array[i];
			service.updateJournals();
		}
*/
}

void Disjunction_construct(Disjunction * const this)
{
	voidPtrMap_create(&this->apps, 		APPS_MAX, 		&this->_appKeys, 	(void *)&this->_apps, 		NULL);
	voidPtrMap_create(&this->devices, 	DEVICES_MAX, 	&this->_deviceKeys, (void *)&this->_devices, 	NULL);
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
	
	//this->initialised = false;
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
	#ifdef DISJUNCTION_DEBUG
	printf("App_update\n");
	#endif
	
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
	Ctrl_updatePost(ctrl); //abstract
}

void App_initialise(App * const this)
{
	this->initialise((void *)this);	
}

void App_start(App * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("App_start\n");
	#endif
	
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
			//this->initialise((void *)this);
		}
		
		this->running = true;
		/*
		if (!ctrl->initialised)
		{
			printf ("App_start - Error: ctrl has not been initialised.\n"); 
			exit(1);
		}
		
		if (!view->initialised)
		{
			printf ("App_start - Error: view has not been initialised.\n"); 
			exit(1);
		}
		*/
		ctrl->start((void *)ctrl);
		view->start((void *)view);
	}
}

void App_stop(App * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("App_stop\n");
	#endif
	
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

void Ctrl_initialise(Ctrl * const this)
{
	if (this->initialise)
		this->initialise(this);
	
	this->initialised = true;
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

//this can be called on construction or on first add to parent
void View_construct(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_construct %s!\n", this->id);
	#endif
	
	voidPtrMap_create(&this->childrenById, VIEW_CHILDREN_MAX, &this->_childrenByIdKeys, (void *)&this->_childrenById, NULL);
	
	this->childrenByZ.entries = (void *) &this->_childrenByZ;
	this->childrenByZ.capacity = sizeof(this->_childrenByZ);
	this->childrenByZ.fail = NULL;
}
	
void View_initialise(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_initialise %s!\n", this->id);
	#endif
	
	if (this->initialise)
		this->initialise(this);
	
	this->initialised = true;
	
	List childrenByZ = this->childrenByZ;
	int length = childrenByZ.length;

	for (int i = 0; i < length; i++)
	{
		View * child = (View *)childrenByZ.entries[i];
		View_initialise(child);//deltaSec //only works if enabled
	}
}

void View_update(View * const this)
{
	this->update(this);//deltaSec

	List childrenByZ = this->childrenByZ;
	int length = childrenByZ.length;
	//printf("try %s %d\n", &this->id, length);

	for (int i = 0; i < length; i++)
	{
		View * child = (View *)childrenByZ.entries[i];
		View_update(child);//deltaSec //only works if enabled
	}
	
	this->updatePost(this);//deltaSec
}

void View_disposeRecurse(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_disposeRecurse\n");
	#endif
	
	//TODO recurse
	this->dispose(this);
	this->initialised = false;
	//free(this);
	//_aligned_free(this);
};

void View_addChild(View * const this, View * const child)
{
	Map childrenById = this->childrenById;
	List childrenByZ = this->childrenByZ;
	
	//TODO should be if canPut() / canAdd() (both have same capacity)
	child->parent = this;
	put(&this->childrenById, &child->id[0], child);
	add(&this->childrenByZ, child); //just add it at the next available slot, i.e. on top by default
}

/*
void View_removeChild(View * const this, View * const child)
{
	Map childrenById  = this->childrenById;
	
	if (put(&childrenById, child->id, child)) //only do the full process if able to add to the map
	//TODO there are more things to check here... e.g. make sure that capcity of both collections is same (do in init())
	{
		child->parent = this;
		List childrenByZ = this->childrenByZ;
		add(&childrenByZ, child);
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

void View_onParentResizeRecurse(View * const this)
{
	this->onParentResize(this);
	
	int length = this->childrenByZ.length;
	for (int i = 0; i < length; i++)
	{
		View * child = this->childrenByZ.entries[i];
		
		//depth first - update child and then recurse to its children
		
		View_onParentResizeRecurse(child);
	}	
}

void doNothing(void * const this){/*printf("doNothing\n");*/}