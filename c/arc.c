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
#ifndef ARC_KHASH_TYPES_OFF
KHASH_DEFINE(StrPtr, 	kh_cstr_t, uintptr_t, kh_str_hash_func, kh_str_hash_equal, 1)
#endif//ARC_KHASH_TYPES_OFF

static khiter_t k;



//--------- Pub/Sub ---------//

/// Construct a Pub(lisher). This is usually called by Ctrl.initialise(), or possibly by Ctrl.update().
Pub * Pub_construct(const char * name)//, void * data)
{
	Pub * pubPtr = calloc(1, sizeof(Pub));
	strcpy(pubPtr->name, name);
	//pubPtr->data = data;
	kv_init(pubPtr->subsList);
	return pubPtr;
}

/// Publish to the Sub(scriber) list.
void Pub_lish(Pub * pubPtr, void * info)
{
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
	kv_push(Sub, pubPtr->subsList, *subPtr);	
}

//--------- Hub ---------//

void Hub_setDefaultCallbacks(Hub * hub)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_setDefaultCallbacks\n");
	#endif
	
	hub->initialise = (void * const)&doNothing;
	hub->dispose 	= (void * const)&doNothing;
	hub->suspend 	= (void * const)&doNothing;
	hub->resume 	= (void * const)&doNothing;
}

void Hub_update(Hub * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Hub_update...\n");
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
	LOGI("[ARC] Hub_construct...\n");
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	Hub * hub = calloc(1, sizeof(Hub));
	//#endif//__GNUC__
	
	Hub_setDefaultCallbacks(hub);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_construct\n");
	#endif
	
	return hub;
}

void Hub_destruct(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_destruct...\n"); 
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
	LOGI("[ARC] Hub_suspend...");
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
	LOGI("[ARC] Hub_resume...");
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
	LOGI("[ARC] Hub_addApp... (app id=%s)\n", app->id);
	#endif
	if (this->appsCount < APPS_MAX)
	{
		this->apps[this->appsCount++] = app;
		app->hub = this;
		//#ifdef ARC_DEBUG_ONEOFFS
		//LOGI("[ARC] App added with ID %s\n", app->id);
		//#endif// ARC_DEBUG_ONEOFFS
		
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC] ...Hub_addApp (app id=%s)\n", app->id);
		#endif
		
		return app;
	}
	return NULL;
}

App * const Hub_getApp(Hub * const this, const char * const id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_getApp... (app id=%s)\n", id);
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
App * App_construct(const char * id)//App ** app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_construct...(id=%s)\n", id);
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	App * app = calloc(1, sizeof(App));
	//#endif//__GNUC__
	strcpy(app->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	app->pubsByName = kh_init(StrPtr);
	app->initialise = (void * const)&doNothing;
	app->dispose 	= (void * const)&doNothing;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_construct (id=%s)\n", id);
	#endif
	
	return app;
}

void App_update(App * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] App_update... (id=%s)\n", this->id);
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
	LOGI("[ARC] ...App_update (id=%s)\n", this->id);
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
	LOGI("[ARC] App_start... (id=%s)\n", this->id);
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
	LOGI("[ARC] ...App_start (id=%s)\n", this->id);
	#endif
}

void App_stop(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_stop... (id=%s)\n", this->id);
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
	LOGI("[ARC] App_destruct... (id=%s)\n", id);
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
	LOGI("[ARC] ...App_destruct (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_suspend(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_suspend... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_suspend(this->view);
	
	this->ctrl->suspend((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_suspend (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_resume(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_resume... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_resume(this->view);

	this->ctrl->resume((void *)this);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_resume (id=%s)\n", this->id);
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
Ctrl * Ctrl_construct(const char * id, size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_construct...(id=%s)\n", id);
	#endif
	LOGI("sizeof ctrl=%u\n", sizeofSubclass);
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	Ctrl * ctrl = calloc(1, sizeofSubclass);
	strcpy(ctrl->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	Ctrl_setDefaultCallbacks(ctrl);
	//kv_init(ctrl->configs);
	ctrl->extensionsById = kh_init(StrPtr);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_construct(id=%s)\n", id);
	#endif
	
	return ctrl;
}

void Ctrl_setDefaultCallbacks(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_setDefaultCallbacks (id=%s)\n", this->id);
	#endif
	
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
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_start (id=%s)\n", this->id);
	#endif
}

void Ctrl_stop(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_stop (id=%s)\n", this->id);
	#endif
}

void Ctrl_suspend(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_suspend... (id=%s)\n", this->id);
	#endif
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_suspend (id=%s)\n", this->id);
	#endif
}

void Ctrl_resume(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_resume... (id=%s)\n", this->id);
	#endif
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_resume (id=%s)\n", this->id);
	#endif
}

void Ctrl_initialise(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_initialise (id=%s)\n", this->id);
	#endif
}

void Ctrl_update(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Ctrl_update... (id=%s)\n", this->id);
	#endif
	
	this->update(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_update (id=%s)\n", this->id);
	#endif
}

void Ctrl_updatePost(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Ctrl_updatePost... (id=%s)\n", this->id);
	#endif
	
	this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_updatePost (id=%s)\n", this->id);
	#endif
}

void Ctrl_destruct(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC] Ctrl_destruct... (id=%s)\n", id);
	#endif
	
	this->dispose(this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_destruct (id=%s)\n", id);
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
	LOGI("[ARC] View_setDefaultCallbacks (id=%s)\n", this->id);
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
	LOGI("[ARC] View_construct... (id=%s)\n", id);
	#endif
	
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	View * view = calloc(1, sizeofSubclass);
	strcpy(view->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	View_setDefaultCallbacks(view);
	kv_init(view->childrenByZ);
	//view->subStatusesByName = kh_init(StrPtr);
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_construct (id=%s)\n", id);
	#endif
	
	return view;
}

void View_start(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_start (id=%s)\n", this->id);
	#endif
}

void View_stop(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_stop (id=%s)\n", this->id);
	#endif
}

void View_suspend(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_suspend... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_suspend(child);
	}
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_suspend (id=%s)\n", this->id);
	#endif
}

void View_resume(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_resume... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_resume(child);
	}
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_resume (id=%s)\n", this->id);
	#endif
}


void View_initialise(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;

	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		View_initialise(child);//deltaSec //only works if enabled
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_initialise (id=%s)\n", this->id);
	#endif
}

void View_update(View * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] View_update... (id=%s)\n", this->id);
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
	LOGI("[ARC] ...View_update (id=%s)\n", this->id);
	#endif
}

void View_destruct(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC] View_destruct... (id=%s)\n", id);
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
	LOGI("[ARC] ...View_destruct (id=%s)\n", id);
	#endif
}

View * View_getChild(View * const this, char * id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_getChild... (id=%s) (child id=%s)\n", this->id, id);
	#endif
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		if (strcmp(id, child->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...View_getChild (id=%s) (child id=%s)\n", this->id, id);
			#endif
			
			return child;
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_getChild (id=%s) (child id=%s)\n", this->id, id);
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
	LOGI("[ARC] View_addChild... (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	View_claimAncestry(this, child);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_addChild (id=%s) (child id=%s)\n", this->id, child->id);
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
	LOGI("[ARC] View_onParentResize... (id=%s)\n", this->id);
	#endif
	
	this->onParentResize(this);
	
	for (int i = 0; i < kv_size(this->childrenByZ); ++i)
	{
		View * child = kv_A(this->childrenByZ, i); //NB! dispose in draw order
		
		//depth first - update child and then recurse to its children
		
		View_onParentResize(child);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_onParentResize (id=%s)\n", this->id);
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

View * Builder_addView(App * app, View * view, ezxml_t viewXml, void * model, ezxml_t modelXml)
{
	View * subview;
	ezxml_t subviewXml;
	const char * name;
	const char * modelClass = ezxml_attr(modelXml, "class");
	const char * viewClass = ezxml_attr(viewXml, "class");
	
	view = View_construct(ezxml_attr(viewXml, "id"), sizeofDynamic(viewClass));
	view->model 			= model;
	
	name = ezxml_attr(viewXml, "onParentResize");
	if (name) view->onParentResize = addressofDynamic(name);
	name = ezxml_attr(viewXml, "start");
	if (name) view->start = addressofDynamic(name);
	name = ezxml_attr(viewXml, "stop");
	if (name) view->stop = addressofDynamic(name);
	name = ezxml_attr(viewXml, "update");
	if (name) view->update = addressofDynamic(name);
	name = ezxml_attr(viewXml, "updatePost");
	if (name) view->updatePost = addressofDynamic(name);
	name = ezxml_attr(viewXml, "initialise");
	if (name) view->initialise = addressofDynamic(name);
	name = ezxml_attr(viewXml, "dispose");
	if (name) view->dispose = addressofDynamic(name);
	name = ezxml_attr(viewXml, "suspend");
	if (name) view->suspend = addressofDynamic(name);
	name = ezxml_attr(viewXml, "resume");
	if (name) view->resume = addressofDynamic(name);

	if (app)
		App_setView(app, view); //must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	
	for (subviewXml = ezxml_child(viewXml, "view"); subviewXml; subviewXml = subviewXml->next)
	{
		subview = Builder_addView(NULL, subview, subviewXml, model, modelXml);

		View_addChild(view, subview);
	}
	
	return view;
}

ezxml_t ezxml_child_any(ezxml_t xml)
{
    xml = (xml) ? xml->child : NULL;
    //while (xml) xml = xml->sibling;
    return xml;
}

App * Builder_buildApp(ezxml_t appXml)
{
	ezxml_t modelXml, viewXml, subviewXml, rootctrlXml, ctrlXml, elementXml, elementXmlCopy;
	const char * modelClass;
	const char * ctrlClass;
	char string[STRLEN_MAX];
	
	void * model;
	View * view;
	Ctrl * ctrl;
	const char * name;
	
	//app (create)
	App * app = App_construct(ezxml_attr(appXml, "id"));
	
	name = ezxml_attr(appXml, "initialise");
	if (name) app->initialise = addressofDynamic(name);
	name = ezxml_attr(appXml, "dispose");
	if (name) app->dispose = addressofDynamic(name);
	name = ezxml_attr(appXml, "suspend");
	if (name) app->suspend = addressofDynamic(name);
	name = ezxml_attr(appXml, "resume");
	if (name) app->resume = addressofDynamic(name);
	
	//model
	modelXml = ezxml_child(appXml, "model");
	modelClass = ezxml_attr(modelXml, "class");
	model = calloc(1, sizeofDynamic(modelClass));
	app->model 		= model;
	
	//views
	viewXml = ezxml_child(appXml, "view");
	view = Builder_addView(app, view, viewXml, model, modelXml);
	#ifdef DESKTOP
	//View_addChild((View *)mainView, (View *)terminalView);
	#endif//DESKTOP
	
	//ctrl
	ctrlXml = ezxml_child(appXml, "ctrl");
	ctrlClass = ezxml_attr(ctrlXml, "class");
	ctrl = Ctrl_construct(ezxml_attr(ctrlXml, "id"), sizeofDynamic(ctrlClass));
	ctrl->model 	= model;
	
	name = ezxml_attr(ctrlXml, "mustStart");
	if (name) ctrl->mustStart = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "mustStop");
	if (name) ctrl->mustStop = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "start");
	if (name) ctrl->start = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "stop");
	if (name) ctrl->stop = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "update");
	if (name) ctrl->update = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "updatePost");
	if (name) ctrl->updatePost= addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "initialise");
	if (name) ctrl->initialise= addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "dispose");
	if (name) ctrl->dispose = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "suspend");
	if (name) ctrl->suspend = addressofDynamic(name);
	name = ezxml_attr(ctrlXml, "resume");
	if (name) ctrl->resume= addressofDynamic(name);
	
	//TODO find custom elements and build them using their name as a key into a map provided for each element type
	for (elementXml = ezxml_child_any(ctrlXml); elementXml; elementXml = elementXml->sibling) //run through distinct child element names
	{
		elementXmlCopy = elementXml;
		while (elementXmlCopy) //iterate over child elements of same name (that sit adjacent?)
		{
			//LOGI("element name is %s", ezxml_name(elementXmlCopy));
			ExtensionFromConfigXML constructor = addressofDynamic(ezxml_attr(elementXmlCopy, "constructor"));
			void * extension = constructor(elementXmlCopy);
			
			strcpy(((Extension *)extension)->id, ezxml_attr(elementXmlCopy, "id"));
			
			kh_set(StrPtr, ctrl->extensionsById, ((Extension *)extension)->id, extension);
			//kv_push(void *, ctrl->extensions, extension);
			elementXmlCopy = elementXmlCopy->next;
		}
	}
	
	App_setCtrl(app, ctrl);
	
	return app;
}

void Builder_buildHub(Hub * hub, ezxml_t hubXml)
{
	const char * name;
	
	name = ezxml_attr(hubXml, "initialise");
	if (name) hub->initialise = addressofDynamic(name);
	name = ezxml_attr(hubXml, "dispose");
	if (name) hub->dispose = addressofDynamic(name);
	name = ezxml_attr(hubXml, "suspend");
	if (name) hub->suspend = addressofDynamic(name);
	name = ezxml_attr(hubXml, "resume");
	if (name) hub->resume = addressofDynamic(name);
	
	ezxml_t appsXml = ezxml_child(hubXml, "apps");
	
	for (ezxml_t appXml = ezxml_child(appsXml, "app"); appXml; appXml = appXml->next)
	{
		App * app = Builder_buildApp(appXml);
		Hub_addApp(hub, app);
	}
}

void Builder_buildFromConfig(Hub * const hub, const char * configFilename)
{
	ezxml_t hubXml = ezxml_parse_file(configFilename);
	Builder_buildHub(hub, hubXml);
	ezxml_free(hubXml);
}

//--------- misc ---------//
void doNothing(void * const this){/*LOGI("[ARC] doNothing\n");*/}
