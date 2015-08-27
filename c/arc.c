#include "arc.h"

//#define DISJUNCTION_DEBUG

//--------- Hub ---------//
void Hub_construct(Hub * const this, int appsCount)
{
}

void Hub_update(Hub * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("Hub_update\n");
	#endif
	
	//update apps
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i]; //read from map of values
		if (app->updating)
			App_update(app);
	}
}

void Hub_dispose(Hub * const this)
{
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_dispose(app);
	}
	
	this->dispose((void *)this);
	
	//this->initialised = false;
	printf ("Hub_dispose done."); 
	//free(this); //hub object is not a pointer!
}

void Hub_suspend(Hub * const this)
{
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_suspend(app);
	}

	this->suspend((void *)this);
	
	printf ("Hub_suspend done.");
}

void Hub_resume(Hub * const this)
{
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_resume(app);
	}
	
	this->resume((void *)this);
	
	printf ("Hub_resume done.");
}

App * const Hub_addApp(Hub * const this, App * const app)
{
	printf("app0");
	if (this->appsCount < APPS_MAX)
	{
		printf("app1");
		this->apps[this->appsCount++] = app;
		printf("app2");
		app->hub = this;
		printf("app->id? %s\n", app->id);
		return app;
	}
	return NULL;
}

App * const Hub_getApp(Hub * const this, const char * const id)
{
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (strcmp(id, app->id) == 0)
			return app;
	}
	return NULL;
}

//--------- App ---------//
App * App_construct()//App ** app)
{
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	App * app = malloc(sizeof(App));
	//#endif//__GNUC__
	*app = appEmpty;
	app->initialise = (void * const)&doNothing;
	app->dispose 	= (void * const)&doNothing;
}

/*
void App_destruct(App ** app) //pointer-to-pointer required for __attribute__ cleanup
{
	LOGI("destruct %s", (*app)->id);
	free(*app);
}
*/
void App_update(App * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("App_update\n");
	#endif
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	Ctrl_update(ctrl); //abstract
	//if (view != NULL) //JIC user turns off the root view by removing it (since this is the enable/disable mechanism)
	//really, we should just exit if either View or Ctrl are null, at App_start()
	if (view->updating)
		View_update(view);
	Ctrl_updatePost(ctrl); //abstract
}

void App_initialise(App * const this)
{
	this->initialise((void *)this);	
	
	Ctrl_initialise(this->ctrl);
	View_initialise(this->view); //initialises all descendants too
}

void App_start(App * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("App_start\n");
	#endif
	
	if (!this->updating)
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
		
		this->updating = true;
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
		
		Ctrl_start(ctrl);
		View_start(view);
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
	
	this->updating = false;
}

void App_dispose(App * const this)
{
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;

	Ctrl_dispose(ctrl);
	View_dispose(view);
	
	//this->services.dispose();
	
	this->dispose((void *)this);
	this->initialised = false;
	free(this);
}

void App_suspend(App * const this)
{	
	View_suspend(this->view);
	
	this->ctrl->suspend((void *)this);
	
	printf ("App_suspend done.");
}

void App_resume(App * const this)
{
	View_resume(this->view);

	this->ctrl->resume((void *)this);

	printf ("App_resume done.");
}

//--------- Ctrl ---------//
Ctrl * Ctrl_construct(size_t sizeofSubclass)
{
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	Ctrl * ctrl = malloc(sizeofSubclass);
	memset(ctrl, 0, 	 sizeofSubclass);
	Ctrl_setDefaultCallbacks(ctrl);
}

void Ctrl_setDefaultCallbacks(Ctrl * const this)
{
	this->mustStart = (void * const)&doNothing;
	this->mustStop 	= (void * const)&doNothing;
	this->start 	= (void * const)&doNothing;
	this->stop 		= (void * const)&doNothing;
	this->suspend 	= (void * const)&doNothing;
	this->resume 	= (void * const)&doNothing;
	this->initialise= (void * const)&doNothing;
	this->dispose 	= (void * const)&doNothing;
	this->update 	= (void * const)&doNothing;
	this->updatePost= (void * const)&doNothing;
}

void Ctrl_start(Ctrl * const this)
{
	this->start((void *)this);
	this->updating = true;
}

void Ctrl_stop(Ctrl * const this)
{
	this->stop((void *)this);
	this->updating = false;
}

void Ctrl_suspend(Ctrl * const this)
{
	this->suspend(this);
}

void Ctrl_resume(Ctrl * const this)
{
	this->resume(this);	
}

void Ctrl_initialise(Ctrl * const this)
{
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

//--------- View ---------//
void View_setDefaultCallbacks(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_construct %s!\n", this->id);
	#endif
	
	//null id
	//null model
	this->start 			= (void * const)&doNothing;
	this->stop 				= (void * const)&doNothing;
	this->suspend 			= (void * const)&doNothing;
	this->resume 			= (void * const)&doNothing;
	this->initialise		= (void * const)&doNothing;
	this->dispose 			= (void * const)&doNothing;
	this->update 			= (void * const)&doNothing;
	this->updatePost		= (void * const)&doNothing;
	this->onParentResize 	= (void * const)&doNothing;
}

View * View_construct(size_t sizeofSubclass)
{
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	View * view = malloc(sizeofSubclass);
	memset(view, 0, 	 sizeofSubclass);
	View_setDefaultCallbacks(view);
}

void View_start(View * const this)
{
	this->start((void *)this);
	this->updating = true;
}

void View_stop(View * const this)
{
	this->stop((void *)this);
	this->updating = false;
}

void View_suspend(View * const this)
{
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_suspend(child);
	}
	
	this->suspend(this);
}

void View_resume(View * const this)
{
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_resume(child);
	}
	
	this->resume(this);	
}


void View_initialise(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_initialise %s!\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;

	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		View_initialise(child);//deltaSec //only works if enabled
	}
}

void View_update(View * const this)
{
	this->update(this);//deltaSec

	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		if (child->updating)
			View_update(child);//deltaSec
	}
	
	this->updatePost(this);//deltaSec
}

void View_dispose(View * const this)
{
	#ifdef DISJUNCTION_DEBUG
	printf("View_dispose\n");
	#endif
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_dispose(child);
	}
	this->dispose(this);
	this->initialised = false;
}

View * View_getChild(View * const this, char * id)
{
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		if (strcmp(id, child->id) == 0)
			return child;
	}
	return NULL;
}

View * View_addChild(View * const this, View * const child)
{
	child->parent = this;

	if (this->childrenCount == VIEW_CHILDREN_MAX)
		return NULL;
	else
	{
		this->childrenByZ[this->childrenCount++] = child;
		return child;
	}
}

/*
View *
View_removeChild(View * const this, View * const child)
{
	child->parent = NULL;

	if (this->childrenCount == 0)
		return ARRAY_EMPTY;
	else
	{
		//TODO find child index
		
		//TODO shift all back
		
		this->childrenCount--;
		return child;
	}
}
*/
/*
ArrayResult
View_swapChildren(View * const this, int indexFrom, int indexTo)
{
	
}
*/

bool View_isRoot(View * const this)
{
	return this->parent == NULL;
}

void View_onParentResizeRecurse(View * const this)
{
	this->onParentResize(this);
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		
		//depth first - update child and then recurse to its children
		
		View_onParentResizeRecurse(child);
	}	
}

//--------- misc ---------//
void doNothing(void * const this){/*printf("doNothing\n");*/}