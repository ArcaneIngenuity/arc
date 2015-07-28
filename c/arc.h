#ifndef ARC_H
#define ARC_H

#include <stdbool.h>
//#include <limits.h> //for INT_MIN
#include <stdlib.h>
#include <stdio.h>

#include "log/log.h"

//(REMOVE ME) collection definitions *before* inclusion of arc.h! (definitions in header lead to multiple definitions due to this: http://stackoverflow.com/questions/14425262/why-include-guards-do-not-prevent-multiple-function-definitions)

#define VIEW_CHILDREN_MAX 8
#define SERVICES_MAX 16
#define APPS_MAX 4
#define DEVICES_MAX 16

/*
typdef enum
ArrayResult
{
	NOT_FOUND = INT_MIN,
	ARRAY_FULL,
	ARRAY_EMPTY
	//any non-negative value means "OK" and may also indicate an index returned (depending on function).
} ArrayResult;
*/

typedef struct DeviceChannel
{
	float value;
	float delta;
} DeviceChannel;

typedef struct Device
{
	int channelsLength;
	struct DeviceChannel * channels;
	bool readEvents;
	
	void (*receive)(struct Device * const this);
	void (*poll)(struct Device * const this);
	void (*flush)(struct Device * const this);
} Device;

//use as base struct for inherited type
typedef struct View
{
	//TODO fix this!
	char * id;
	
	int width;
	int height;
	
	struct View * parent;
	struct View * childrenByZ[VIEW_CHILDREN_MAX];
	int childrenCount; //negative for invalid return values (e.g. on seek)
	
	bool running;
	bool initialised; //true after first start
	void * model;
	
	void (*start)(struct View * const this);
	void (*stop)(struct View * const this);
	void (*initialise)(struct View * const this);
	void (*dispose)(struct View * const this);
	void (*update)(struct View * const this);
	void (*updatePost)(struct View * const this);
	void (*onParentResize)(struct View * const this);
	//void (*enable)(struct View * const this); //start
	//void (*disable)(struct View * const this); //stop
	//bool running; //start/stop
} View;
const struct View viewEmpty;

typedef struct Ctrl
{
	bool running; //start/stop
	bool initialised; //true after first start
	void * model;
	
	void (*mustStart)(struct Ctrl * const this);
	void (*mustStop)(struct Ctrl * const this);
	void (*start)(struct Ctrl * const this);
	void (*stop)(struct Ctrl * const this);
	void (*initialise)(struct Ctrl * const this);
	void (*dispose)(struct Ctrl * const this);
	void (*update)(struct Ctrl * const this);
	void (*updatePost)(struct Ctrl * const this);
} Ctrl;
const struct Ctrl ctrlEmpty;

typedef struct App
{
	char * id; //for compound apps
	struct Hub * hub; //in spite of typedef, use struct due to circular ref App->Hub TODO remove this ref, and allow Apps to send messages up to DJ?
	//struct Map services;
	
	bool running; //start/stop
	bool initialised; //true after first start
	void * model; //cannot know class / size; struct YourModel must contain void * journals[]
	struct View * view; //cannot know class / size
	struct Ctrl * ctrl; //cannot know class / size
	
	//void * external; //anything we had to externally initialise / dispose of, but need a ref to inside App.
	//void (*input)(struct App * const this);
	void (*initialise)(struct App * const this);
	void (*dispose)(struct App * const this);
} App;
const struct App appEmpty;

typedef struct Pointer
{
	Device * device;
	View * target;
	View * targetLast;
	bool selected;
	bool selectedLast;
} Pointer;

typedef struct Service
{
	struct App * app;
	void * models;
} Service;

typedef struct Hub
{
	App * apps[APPS_MAX]; //malloc'd array of pointers to apps -- allows ad-hoc allocation or batched pre-allocation
	int appsCount;
	
	//struct Service _services[SERVICES_MAX];
	//Key _serviceKeys[SERVICES_MAX];

	//struct Pointer pointer;
	//TODO Builder
	
	void * external; //anything we had to externally initialise / dispose of, but need a ref to inside App.
	//void (*start)(struct Hub * const this); 
	//void (*stop)(struct Hub * const this);
	void (*initialise)(struct Hub * const this);
	void (*dispose)(struct Hub * const this);
	
} Hub;
const struct Hub disjunctionEmpty;

//FINAL/BASE METHODS

void Device_constructor(Device * this, int channelCount);
void Device_poll(Device * this);
//void Device_flush(Device * this);
void DeviceChannel_constructor(Device * this);

void Pointer_updateSelected(Pointer * const this);
void Pointer_hasChangedTarget(Pointer * const this);
void Pointer_hasChangedSelected(Pointer * const this);
void Pointer_hasMoved(Pointer * const this);

//bool 		Ctrl_mustStart(Ctrl * const this);
//bool 		Ctrl_mustStop(Ctrl * const this);
void 		Ctrl_start(Ctrl * const this);
void 		Ctrl_stop(Ctrl * const this);
void		Ctrl_initialise(Ctrl * const this);
void 		Ctrl_update(Ctrl * const this);
void 		Ctrl_updatePost(Ctrl * const this);
void 		Ctrl_dispose(Ctrl * const this);
void 		Ctrl_disposeRecurse(Ctrl * const this);

void 		View_start(View * const this);
void 		View_stop(View * const this);
void 		View_construct(View * const this);
void 		View_initialise(View * const this);
void 		View_update(View * const this);
void 		View_disposeRecurse(View * const this);
bool 		View_isRoot(View * const this);
void 		View_onParentResizeRecurse(View * const this);
View * 		View_getChild(View * const this, char * id);
View *		View_addChild(View * const this, View * const child);
//
//TODO... View * View_removeChild(View * const this, View * const child); //first get child by ID
//ArrayResult View_swapChildren(View * const this, int indexFrom, int indexTo);

void 		App_initialise(App * const this);
void 		App_update(App * const this);
void 		App_start(App * const this);
void 		App_stop(App * const this);
void 		App_dispose(App * const this);
//TODO... void App_addService(App * const this, const char * id, Service * service);
//TODO... void App_removeService(App * const this, const char * id);

void 		Hub_construct(Hub * const this, int appsCount);
void 		Hub_initialise(Hub * const this);
void 		Hub_dispose(Hub * const this);
void 		Hub_update(Hub * const this);
App * const Hub_addApp(Hub * const this, App * app);
App * const Hub_getApp(Hub * const this, const char * const id);
//TODO... void Hub_removeApp(Hub * const this, const char * id);
//TODO... void Hub_addDevice(Hub * const this, const char * id, Device * const device);
//TODO... void Hub_removeDevice(Hub * const this, const char * id);

void doNothing(void * const this);

Hub hub;// = {0}; //could use an empty?
#endif //ARC_H