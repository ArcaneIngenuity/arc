/// \copyright Copyright 2011-2015 N.P. Wiggill <nick.wiggill@gmail.com>. All rights reserved.
/// 
/// \license This project is released under the <a href="https://opensource.org/licenses/MIT">MIT License</a>.
/// 
/// Permission is hereby granted, free of charge, to any person obtaining
/// a copy of this software and associated documentation files (the
/// "Software"), to deal in the Software without restriction, including
/// without limitation the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the Software, and to
/// permit persons to whom the Software is furnished to do so, subject to
/// the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
/// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
/// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// \brief arc - a realtime applications framework for native & web

#include "arc.h"

#define ARC_DEBUG_ONEOFFS 1
//#define ARC_DEBUG_UPDATES 1
//#define ARC_DEBUG_PUBSUB 1
#ifndef ARC_KHASH_TYPES_OFF
KHASH_DEFINE(StrPtr, 	kh_cstr_t, uintptr_t, kh_str_hash_func, kh_str_hash_equal, 1)
#endif//ARC_KHASH_TYPES_OFF

static khiter_t k;



//--------- Pub/Sub ---------//

/// Construct a Pub(lisher). This is usually called by Ctrl.initialise(), or possibly by Ctrl.update().
Pub * Pub_construct(const char * name)//, void * data)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Pub_construct (name=%name)\n", name);
	#endif
	Pub * pubPtr = calloc(1, sizeof(Pub));
	strcpy(pubPtr->name, name);
	//pubPtr->data = data;
	kv_init(pubPtr->subsList);
	return pubPtr;
}

/// Publish to the Sub(scriber) list.
void Pub_lish(Pub * pubPtr, void * info)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Pub_lish\n");
	#endif
	for (int i = 0; i < kv_size(pubPtr->subsList); ++i)
	{
		Sub * subPtr = &kv_A(pubPtr->subsList, i);
		subPtr->handler(subPtr->instance, info);
	}
}

/// Sub(scribe) to the Pub(lisher).

/// A Sub(scriber) knows what the data
void Sub_scribe(Sub * subPtr, Pub * pubPtr)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Sub_scribe\n");
	#endif
	kv_push(Sub, pubPtr->subsList, *subPtr);	
}

//--------- Hub ---------//

void Hub_setDefaultCallbacks(Hub * hub)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_setDefaultCallbacks\n");
	#endif
	
	hub->initialise = (void * const)&doNothing;
	hub->dispose 	= (void * const)&doNothing;
	hub->suspend 	= (void * const)&doNothing;
	hub->resume 	= (void * const)&doNothing;
}

void Hub_update(Hub * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Hub_update...\n");
	#endif
	
	//update apps
	for (int i = 0; i < this->appsCount; ++i)
	{
		App * app = this->apps[i]; //read from map of values
		if (app->updating)
			App_update(app);
	}
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Hub_update\n");
	#endif
}

Hub * Hub_construct()
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_construct...\n");
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	Hub * hub = calloc(1, sizeof(Hub));
	//#endif//__GNUC__
	hub->extensionsById = kh_init(StrPtr);
	kv_init(hub->extensionIds);
	//Hub_setDefaultCallbacks(hub);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_construct\n");
	#endif
	
	return hub;
}

void Hub_destruct(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_destruct...\n"); 
	#endif// ARC_DEBUG_ONEOFFS
	
	for (int i = 0; i < this->appsCount; ++i)
	{
		App * app = this->apps[i];
		if (app)
			App_destruct(app);
	}
	
	this->dispose((void *)this);
	
	//this->initialised = false;
	free(this); //hub object is not a pointer!
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_destruct\n"); 
	#endif// ARC_DEBUG_ONEOFFS
}

void Hub_suspend(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_suspend...");
	#endif// ARC_DEBUG_ONEOFFS
	for (int i = 0; i < this->appsCount; ++i)
	{
		App * app = this->apps[i];
		if (app)
			App_suspend(app);
	}

	this->suspend((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_suspend");
	#endif// ARC_DEBUG_ONEOFFS
}

void Hub_resume(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_resume...");
	#endif// ARC_DEBUG_ONEOFFS
	for (int i = 0; i < this->appsCount; ++i)
	{
		App * app = this->apps[i];
		if (app)
			App_resume(app);
	}
	
	this->resume((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_resume");
	#endif// ARC_DEBUG_ONEOFFS
}

App * const Hub_addApp(Hub * const this, App * const app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_addApp... (id=%s)\n", app->id);
	#endif
	if (this->appsCount < APPS_MAX)
	{
		this->apps[this->appsCount++] = app;
		app->hub = this;
		//#ifdef ARC_DEBUG_ONEOFFS
		//LOGI("[ARC] App added with ID %s\n", app->id);
		//#endif// ARC_DEBUG_ONEOFFS
		
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC] ...Hub_addApp    (id=%s)\n", app->id);
		#endif
		
		return app;
	}
	return NULL;
}

App * const Hub_getApp(Hub * const this, const char * const id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Hub_getApp... (app id=%s)\n", id);
	#endif
	
	for (int i = 0; i < this->appsCount; ++i)
	{
		App * app = this->apps[i];
		if (strcmp(id, app->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...Hub_getApp (app id=%s)\n", id);
			#endif
			
			return app;
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_getApp (app id=%s)\n", id);
	#endif
	
	return NULL;
}

//--------- App ---------//
void App_setDefaultCallbacks(App * app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_setDefaultCallbacks\n");
	#endif
	
	app->initialise = (void * const)&doNothing;
	app->dispose 	= (void * const)&doNothing;
	app->suspend 	= (void * const)&doNothing;
	app->resume 	= (void * const)&doNothing;
}


App * App_construct(const char * id)//App ** app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_construct...(id=%s)\n", id);
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	App * app = calloc(1, sizeof(App));
	//#endif//__GNUC__
	strcpy(app->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	app->extensionsById = kh_init(StrPtr);
	kv_init(app->extensionIds);
	app->pubsByName = kh_init(StrPtr);
	//App_setDefaultCallbacks(app);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_construct    (id=%s)\n", id);
	#endif
	
	return app;
}

void App_update(App * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    App_update... (id=%s)\n", this->id);
	#endif
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	Ctrl_update(ctrl); //abstract
	//if (view != NULL) //JIC user turns off the root view by removing it (since this is the enable/disable mechanism)
	//really, we should just exit if either View or Ctrl are null, at App_start()
	if (view->updating)
		View_update(view);
	Ctrl_updatePost(ctrl); //abstract
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...App_update    (id=%s)\n", this->id);
	#endif
}

void App_initialise(App * const this)
{
	this->initialise((void *)this);
	this->initialised = true;
	
	Ctrl_initialise(this->ctrl);
	View_initialise(this->view); //initialises all descendants too
}

void App_start(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_start... (id=%s)\n", this->id);
	#endif
	if (!this->updating)
	{
		if (this->initialised)
		{
			Ctrl * ctrl = this->ctrl;
			Ctrl_start(ctrl); //it is left to Ctrls to start Views
			this->updating = true;
		}
		else
		{
			//TODO make all LOGE, not LOGI!
			LOGI("[ARC] App_start - Error: App has not been initialised.\n"); 
		}
	}
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_start    (id=%s)\n", this->id);
	#endif
}

void App_stop(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_stop... (id=%s)\n", this->id);
	#endif
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	Ctrl_stop(ctrl);
	View_stop(view);
	
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_stop (id=%s)\n", this->id);
	#endif
}

void App_destruct(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC]    App_destruct... (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;

	Ctrl_destruct(ctrl);
	View_destruct(view);
	
	//this->services.dispose();
	
	this->dispose((void *)this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_destruct    (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_suspend(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_suspend... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_suspend(this->view);
	
	this->ctrl->suspend((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_suspend    (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_resume(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    App_resume... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_resume(this->view);

	this->ctrl->resume((void *)this);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_resume    (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_setCtrl(App * app, Ctrl * ctrl)
{
	app->ctrl = ctrl;
	ctrl->app = app;
	ctrl->hub = app->hub;
}

void App_setView(App * app, View * view)
{
	app->view = view;
	view->app = app;
	view->hub = app->hub;
}

//--------- Ctrl ---------//
void Ctrl_setDefaultCallbacks(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_setDefaultCallbacks (id=%s)\n", this->id);
	#endif
	
	//this->mustStart = (void * const)&doNothing;
	//this->mustStop 	= (void * const)&doNothing;
	this->start 	= (void * const)&doNothing;
	this->stop 		= (void * const)&doNothing;
	this->suspend 	= (void * const)&doNothing;
	this->resume 	= (void * const)&doNothing;
	this->initialise= (void * const)&doNothing;
	this->dispose 	= (void * const)&doNothing;
	this->update 	= (void * const)&doNothing;
	this->updatePost= (void * const)&doNothing;
}

Ctrl * Ctrl_construct(const char * id, size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_construct... (id=%s)\n", id);
	#endif
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	Ctrl * ctrl = calloc(1, sizeofSubclass);
	strcpy(ctrl->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	//Ctrl_setDefaultCallbacks(ctrl);
	//kv_init(ctrl->configs);
	ctrl->extensionsById = kh_init(StrPtr);
	kv_init(ctrl->extensionIds);
	//ctrl->construct(ctrl);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_construct    (id=%s)\n", id);
	#endif
	
	return ctrl;
}

void Ctrl_start(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_start    (id=%s)\n", this->id);
	#endif
}

void Ctrl_stop(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_stop    (id=%s)\n", this->id);
	#endif
}

void Ctrl_suspend(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_suspend... (id=%s)\n", this->id);
	#endif
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_suspend    (id=%s)\n", this->id);
	#endif
}

void Ctrl_resume(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_resume... (id=%s)\n", this->id);
	#endif
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_resume    (id=%s)\n", this->id);
	#endif
}

void Ctrl_initialise(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_initialise    (id=%s)\n", this->id);
	#endif
}

void Ctrl_update(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Ctrl_update... (id=%s)\n", this->id);
	#endif
	
	this->update(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_update    (id=%s)\n", this->id);
	#endif
}

void Ctrl_updatePost(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Ctrl_updatePost... (id=%s)\n", this->id);
	#endif
	
	this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_updatePost    (id=%s)\n", this->id);
	#endif
}

void Ctrl_destruct(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC]    Ctrl_destruct... (id=%s)\n", id);
	#endif
	
	this->dispose(this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_destruct    (id=%s)\n", id);
	#endif
}

void Ctrl_createPub(Ctrl * this, const char * name)
{
	App * appPtr = this->app;
	Pub * pubPtr = Pub_construct(name);
	kh_set(StrPtr, appPtr->pubsByName, name, pubPtr);
}

//--------- View ---------//
void View_setDefaultCallbacks(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_setDefaultCallbacks (id=%s)\n", this->id);
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

View * View_construct(const char * id, size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_construct... (id=%s)\n", id);
	#endif
	
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	View * view = calloc(1, sizeofSubclass);
	strcpy(view->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	//View_setDefaultCallbacks(view);
	kv_init(view->childrenByZ);
	view->extensionsById = kh_init(StrPtr);
	kv_init(view->extensionIds);
	//view->subStatusesByName = kh_init(StrPtr);
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_construct    (id=%s)\n", id);
	#endif
	
	return view;
}

void View_start(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_start    (id=%s)\n", this->id);
	#endif
}

void View_stop(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_stop    (id=%s)\n", this->id);
	#endif
}

void View_suspend(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_suspend... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_suspend(child);
	}
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_suspend    (id=%s)\n", this->id);
	#endif
}

void View_resume(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_resume... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_resume(child);
	}
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_resume    (id=%s)\n", this->id);
	#endif
}


void View_initialise(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;

	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_initialise(child);//deltaSec //only works if enabled
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_initialise    (id=%s)\n", this->id);
	#endif
}

void View_update(View * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    View_update... (id=%s)\n", this->id);
	#endif
	
	this->update(this);//deltaSec

	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		if (child->updating)
			View_update(child);//deltaSec
	}
	
	this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...View_update    (id=%s)\n", this->id);
	#endif
}

void View_destruct(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC]    View_destruct... (id=%s)\n", id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_destruct(child);
	}
	
	kv_destroy(this->childrenByZ);
	this->dispose(this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_destruct    (id=%s)\n", id);
	#endif
}

View * View_getChild(View * const this, char * id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_getChild... (id=%s) (child id=%s)\n", this->id, id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		if (strcmp(id, child->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...View_getChild    (id=%s) (child id=%s)\n", this->id, id);
			#endif
			
			return child;
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_getChild    (id=%s) (child id=%s)\n", this->id, id);
	#endif
	
	return NULL;
}

void View_claimAncestry(View * const this, View * const child)
{
	child->parent = this;
	if (this->root)
		child->root = this->root;
	else
		child->root = this;
	child->app = this->app;
	child->hub = this->hub;
	
	//recurse
	for (int i = 0; i < kv_size(child->childrenByZ); ++i)
	{
		View * grandchild = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_claimAncestry(child, grandchild);
	}
}

View * View_addChild(View * const this, View * const child)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_addChild... (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	View_claimAncestry(this, child);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_addChild    (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	kv_push(View *, this->childrenByZ, child); //NB! dispose in draw order
	//this->childrenByZ[this->childrenCount++] = child;
	return child;
}

/*
View *
View_removeChild(View * const this, View * const child)
{
	child->parent = NULL;

	if (kv_size(this->childrenByZ) == 0)
		return ARRAY_EMPTY;
	else
	{
		//TODO find child index
		
		//TODO shift all back
		
		kv_size(this->childrenByZ)--;
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

void View_onParentResize(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_onParentResize... (id=%s)\n", this->id);
	#endif
	
	this->onParentResize(this);
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		
		//depth first - update child and then recurse to its children
		
		View_onParentResize(child);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_onParentResize    (id=%s)\n", this->id);
	#endif
}

void View_subscribe(View * this, const char * pubname, SubHandler handler)
{
	App * app = this->app;
	
	//get the publisher in question off the app
	k = kh_get(StrPtr, app->pubsByName, pubname);
	Pub * pubPtr = kh_val(app->pubsByName, k);
	
	//add this and its handler to the Pub as a Sub (copy local scope struct data into subsList entry)
	Sub sub;
	sub.instance = this;
	sub.handler = handler;
	Sub_scribe(&sub, pubPtr);
}

void View_listen(View * const this)
{
	
	
}

//-------- Builder -------//

typedef void * (*BuildFunction) (ezxml_t xml);

#define FOREACH_CTRL_FUNCTION(HANDLER, T) \
	HANDLER(start, T) \
	HANDLER(stop, T) \
	HANDLER(update, T) \
	HANDLER(updatePost, T) \
	HANDLER(initialise, T) \
	HANDLER(dispose, T) \
	HANDLER(suspend, T) \
	HANDLER(resume, T)
	//HANDLER(construct, T) \
	//HANDLER(destruct, T)
	//HANDLER(mustStart, T) \
	//HANDLER(mustStop, T) \

#define FOREACH_VIEW_FUNCTION(HANDLER, T) \
	HANDLER(onParentResize, T) \
	HANDLER(start, T) \
	HANDLER(stop, T) \
	HANDLER(update, T) \
	HANDLER(updatePost, T) \
	HANDLER(initialise, T) \
	HANDLER(dispose, T) \
	HANDLER(suspend, T) \
	HANDLER(resume, T)
	
#define FOREACH_HUB_FUNCTION(HANDLER, T) \
	HANDLER(initialise, T) \
	HANDLER(dispose, T) \
	HANDLER(suspend, T) \
	HANDLER(resume, T)
	
#define FOREACH_APP_FUNCTION(HANDLER, T) \
	HANDLER(initialise, T) \
	HANDLER(dispose, T) \
	HANDLER(suspend, T) \
	HANDLER(resume, T)

#define GENERATE_ASSIGN_METHOD(member, instance) name = ezxml_attr(instance##Xml, #member); \
	if (name) instance->member = addressofDynamic(name); \
	else \
	{ \
		strcpy(nameAssembled, instance##Class); \
		strcat(nameAssembled, "_"); \
		strcat(nameAssembled, #member); \
		instance->member = addressofDynamic(nameAssembled); \
	} \
	if (!instance->member) \
	{ \
		instance->member = &doNothing; \
		LOGI("[ARC]    Using default function: doNothing.\n"); \
	}

View * Builder_buildView(App * app, View * view, ezxml_t viewXml, void * model)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildView...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	char nameAssembled[STRLEN_MAX];
	const char * name;
	const char * viewClass = ezxml_attr(viewXml, "class");
	
	view = View_construct(ezxml_attr(viewXml, "id"), sizeofDynamic(viewClass));
	view->model = model;
	
	FOREACH_VIEW_FUNCTION(GENERATE_ASSIGN_METHOD, view)
	
	Builder_buildExtensions(viewXml, view->extensionsById, &view->extensionIds);
	
	if (app)
		App_setView(app, view); //must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	
	View * subview;
	for (ezxml_t subviewXml = ezxml_child(viewXml, "view"); subviewXml; subviewXml = subviewXml->next)
	{
		subview = Builder_buildView(NULL, subview, subviewXml, model);

		View_addChild(view, subview);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildView   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return view;
}

Ctrl * Builder_buildCtrl(App * app, ezxml_t ctrlXml, void * model)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildCtrl...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	char nameAssembled[STRLEN_MAX];
	const char * name;
	const char * ctrlClass = ezxml_attr(ctrlXml, "class");
	
	Ctrl * ctrl = Ctrl_construct(ezxml_attr(ctrlXml, "id"), sizeofDynamic(ctrlClass));
	ctrl->model = model;
	
	FOREACH_CTRL_FUNCTION(GENERATE_ASSIGN_METHOD, ctrl)
	
	Builder_buildExtensions(ctrlXml, ctrl->extensionsById, &ctrl->extensionIds);
	
	App_setCtrl(app, ctrl);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildCtrl   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return ctrl;
}

void Builder_buildExtension(ezxml_t extensionXml, khash_t(StrPtr) * extensionsById, kvec_t(ArcString) * extensionIds)
{
	char * extensionId = ezxml_attr(extensionXml, "id");
	char * extensionClassName = ezxml_attr(extensionXml, "class");
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildExtension... (id=%s class=%s)\n", extensionId, extensionClassName);
	#endif// ARC_DEBUG_ONEOFFS
	
	void * extension = calloc(1, sizeofDynamic(extensionClassName));

	//check for parser and run it if available
	//would allow custom <extension initialise="someFunction"> to override this if user desires, but then
	//we'd still have to match the function signature so why not just match name, too, as done here.
	//essentially, an "instance" is only an "extension" if it matches the required function interface.
	char parserFunctionName[STRLEN_MAX];
	strcpy(parserFunctionName, extensionClassName);
	strcat(parserFunctionName, "_fromConfigXML");
	ParserFunctionXML parser = addressofDynamic(parserFunctionName);
	
	if (parser) //if extension constructor is available
	{
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC]    Parser function found: %s :: %s.\n", extensionClassName, parserFunctionName);
		LOGI("[ARC]    Parsing...\n", parserFunctionName, extensionClassName);
		#endif// ARC_DEBUG_ONEOFFS
		parser(extension, extensionXml);
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC] ...Parsing\n", parserFunctionName, extensionClassName);
		#endif// ARC_DEBUG_ONEOFFS
	}
	else //cannot construct extension without function
	{
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC]    Parser function  not found: %s :: %s.\n", extensionClassName, parserFunctionName);
		#endif// ARC_DEBUG_ONEOFFS
	}

	//get extension id and store both this key and its associated value.
	//because we already resized Builder_buildExtensions, no issues here with kvec realloc causing  //problems with string key into khash taken from kvec.
	ArcString s;
	strncpy(s.name, extensionId, STRLEN_MAX);
	kv_push(ArcString, *extensionIds, s); //copy value
	kh_set(StrPtr, extensionsById, kv_A(*extensionIds, kv_size(*extensionIds)-1).name, extension);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildExtension    (id=%s class=%s)\n", extensionId, extensionClassName);
	#endif// ARC_DEBUG_ONEOFFS
}

void Builder_buildExtensions(ezxml_t xml, khash_t(StrPtr) * extensionsById, kvec_t(ArcString) * extensionIds)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildExtensions...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	ezxml_t elementXml, elementXmlCopy;
	
	//first count the elements and resize the vec once-off - this prevents issues
	//with using the kvec's elements as keys into extensionsById (due to realloc)
	size_t count = 0;
	for (elementXml = ezxml_child_any(xml); elementXml; elementXml = elementXml->ordered) //run through distinct child element names
	{
		bool allowCustomElementsAsExtensions = false; //DEV -get from <hub> as an arg (or pass hub as arg)
		
		if (allowCustomElementsAsExtensions)
		{
			//TODO similar to below block, but using a custom element name as extension class name
		}
		else //do not allow custom elements - fallback to seeking standard <extension> elements
		{
			if (strcmp(ezxml_name(elementXml), "extension") == 0) 
				++count;
		}
	}
	kv_resize(ArcString, *extensionIds, count);
	kh_resize(StrPtr,   extensionsById, count);
	//TODO find custom elements and build them using their name as a key into a map provided for each element type
	for (elementXml = ezxml_child_any(xml); elementXml; elementXml = elementXml->sibling) //run through distinct child element names
	{
		bool allowCustomElementsAsExtensions = false; //DEV -get from <hub> as an arg (or pass hub as arg)
		
		if (allowCustomElementsAsExtensions)
		{
			//TODO similar to below block, but using a custom element name as extension class name
		}
		else //do not allow custom elements - fallback to seeking standard <extension> elements
		{
			if (strcmp(ezxml_name(elementXml), "extension") == 0) 
			{
				elementXmlCopy = elementXml;
				while (elementXmlCopy) //iterate over child elements of same name (that sit adjacent?)
				{
					Builder_buildExtension(elementXmlCopy, extensionsById, extensionIds);

					elementXmlCopy = elementXmlCopy->next;
				}
			}
		}
	}
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildExtensions   \n");
	#endif// ARC_DEBUG_ONEOFFS
}

App * Builder_buildApp(ezxml_t appXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildApp...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	ezxml_t modelXml, viewXml, subviewXml, rootctrlXml, ctrlXml;
	char nameAssembled[STRLEN_MAX];
	const char * name;
	const char * modelClass;
	const char * ctrlClass;
	const char * appClass;
	
	void * model;
	View * view;
	Ctrl * ctrl;
	
	//app (create)
	
	appClass = ezxml_attr(appXml, "class");
	App * app = App_construct(ezxml_attr(appXml, "id"));
	
	FOREACH_APP_FUNCTION(GENERATE_ASSIGN_METHOD, app)
	
	Builder_buildExtensions(appXml, app->extensionsById, &app->extensionIds);

	//model
	modelXml = ezxml_child(appXml, "model");
	modelClass = ezxml_attr(modelXml, "class");
	model = calloc(1, sizeofDynamic(modelClass));
	app->model 		= model;
	
	//views
	viewXml = ezxml_child(appXml, "view");
	view = Builder_buildView(app, view, viewXml, model);
	
	//ctrl
	ctrlXml = ezxml_child(appXml, "ctrl");
	ctrl = Builder_buildCtrl(app, ctrlXml, model);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildApp   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return app;
}

void Builder_buildHub(Hub * hub, ezxml_t hubXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildHub...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	char nameAssembled[STRLEN_MAX];
	const char * name;
	const char * hubClass = "Hub";
	
	FOREACH_HUB_FUNCTION(GENERATE_ASSIGN_METHOD, hub)
	Builder_buildExtensions(hubXml, hub->extensionsById, &hub->extensionIds);
	ezxml_t appsXml = ezxml_child(hubXml, "apps");
	
	for (ezxml_t appXml = ezxml_child(appsXml, "app"); appXml; appXml = appXml->next)
	{
		App * app = Builder_buildApp(appXml);
		Hub_addApp(hub, app);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildHub   \n");
	#endif// ARC_DEBUG_ONEOFFS
}

void Builder_buildFromConfig(Hub * const hub, const char * configFilename)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildFromConfig...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	ezxml_t hubXml = ezxml_parse_file(configFilename);
	Builder_buildHub(hub, hubXml);
	ezxml_free(hubXml);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildFromConfig   \n");
	#endif// ARC_DEBUG_ONEOFFS
}

//--------- misc ---------//
void doNothing(void * const this){/*LOGI("[ARC] doNothing\n");*/}
