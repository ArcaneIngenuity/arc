#ifndef DISJUNCTION_H
#define DISJUNCTION_H

#define VIEW_CHILDREN_MAX 8
#define SERVICES_MAX 16
#define APPS_MAX 4
#define DEVICES_MAX 16

#include <stdbool.h>
#include <time.h>

#include "../../curt/list_generic.h"
#include "../../curt/map_generic.h"

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
	
	void (*receive)(void * const this);
	void (*poll)(void * const this);
	void (*flush)(void * const this);
} Device;

//use as base struct for inherited type
typedef struct View
{
	char id[8+1]; //as per chosen key size plus null terminator

	struct View * parent;
	
	Map childrenById; //for user convenience, and because builder is a runtime process
	List childrenByZ; //we render back to front of course, thus from end to start of this.
	
	Key _childrenByIdKeys[VIEW_CHILDREN_MAX];
	struct View * _childrenById[VIEW_CHILDREN_MAX];
	
	struct View * _childrenByZ[VIEW_CHILDREN_MAX];
	
	bool initialised; //true after first start
	void * model;
	
	void (*start)(void * const this);
	void (*stop)(void * const this);
	void (*initialise)(void * const this);
	void (*dispose)(void * const this);
	void (*update)(void * const this);
	void (*updatePost)(void * const this);
	//void (*enable)(void * const this); //start
	//void (*disable)(void * const this); //stop
	//bool running; //start/stop
} View;
const struct View viewEmpty;

typedef struct Ctrl
{
	//TODO data members
	bool initialised; //true after first start
	void * model;
	
	void (*mustStart)(void * const this);
	void (*mustStop)(void * const this);
	void (*start)(void * const this);
	void (*stop)(void * const this);
	void (*initialise)(void * const this);
	void (*dispose)(void * const this);
	void (*update)(void * const this);
	void (*updatePost)(void * const this);
} Ctrl;
const struct Ctrl ctrlEmpty;

typedef struct App
{
	char * id; //for compound apps
	struct Disjunction * disjunction; //in spite of typedef, use struct due to circular ref App->Disjunction TODO remove this ref, and allow Apps to send messages up to DJ?
	struct Map services;
	
	bool running; //start/stop
	bool initialised; //true after first start
	void * model; //cannot know class / size; struct YourModel must contain void * journals[]
	struct View * view; //cannot know class / size
	struct Ctrl * ctrl; //cannot know class / size
	
	//void * external; //anything we had to externally initialise / dispose of, but need a ref to inside App.
	//void (*input)(void * const this);
	void (*initialise)(void * const this);
	void (*dispose)(void * const this);
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

typedef struct Disjunction
{
	struct Map apps;
	struct Map devices;
	
	struct App _apps[APPS_MAX];
	Key _appKeys[APPS_MAX];
	
	struct Device _devices[DEVICES_MAX];
	Key _deviceKeys[DEVICES_MAX];
	
	//struct Service _services[SERVICES_MAX];
	//Key _serviceKeys[SERVICES_MAX];
	
	bool initialised; //true after first start

	struct Pointer pointer;
	//TODO Builder
	
	void * external; //anything we had to externally initialise / dispose of, but need a ref to inside App.
	//void (*start)(void * const this); 
	//void (*stop)(void * const this);
	void (*initialise)(void * const this);
	void (*dispose)(void * const this);
	
} Disjunction;
const struct Disjunction disjunctionEmpty;

//FINAL/BASE METHODS

void Device_constructor(Device * this, int channelCount);
void Device_poll(Device * this);
//void Device_flush(Device * this);
void DeviceChannel_constructor(Device * this);

void Pointer_updateSelected(Pointer * const this);
void Pointer_hasChangedTarget(Pointer * const this);
void Pointer_hasChangedSelected(Pointer * const this);
void Pointer_hasMoved(Pointer * const this);

//bool Ctrl_mustStart(Ctrl * const this);
//bool Ctrl_mustStop(Ctrl * const this);
//void Ctrl_start(Ctrl * const this);
//void Ctrl_stop(Ctrl * const this);
void Ctrl_initialise(Ctrl * const this);
void Ctrl_update(Ctrl * const this);
void Ctrl_updatePost(Ctrl * const this);
void Ctrl_dispose(Ctrl * const this);
void Ctrl_disposeRecurse(Ctrl * const this);

void View_construct(View * const this);
void View_initialise(View * const this);
void View_update(View * const this);
void View_disposeRecurse(View * const this);
bool View_isRoot(View * const this);
void View_addChild(View * const this, View * const child);
//TODO...
//void View_removeChild(View * const this, View * const child); //first get child by ID
void View_swapChildren(View * const this, int indexFrom, int indexTo);

void App_initialise(App * const this);
void App_update(App * const this);
void App_start(App * const this);
void App_stop(App * const this);
void App_dispose(App * const this);
//TODO...
void App_addService(App * const this, const char * id, Service * service);
void App_removeService(App * const this, const char * id);

void Disjunction_construct(Disjunction * const this);
void Disjunction_initialise(Disjunction * const this);
void Disjunction_dispose(Disjunction * const this);
void Disjunction_update(Disjunction * const this);
void Disjunction_addApp(Disjunction * const this, const char * id, App * const app);
//TODO...
void Disjunction_removeApp(Disjunction * const this, const char * id);
void Disjunction_addDevice(Disjunction * const this, const char * id, Device * const device);
void Disjunction_removeDevice(Disjunction * const this, const char * id);

void NullFunction(void * const this);

Disjunction disjunction;// = {0}; //could use an empty?
#endif //DISJUNCTION_H