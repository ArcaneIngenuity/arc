#ifndef DISJUNCTION_H
#define DISJUNCTION_H

#define VIEW_CHILDREN_MAX 8
#define SERVICES_MAX 16
#define APPS_MAX 4
#define DEVICES_MAX 16

#include <stdbool.h>

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
	
	void (*receive)(struct Device * const this);
	void (*poll)(struct Device * const this);
	void (*flush)(struct Device * const this);
} Device;

//use as base struct for inherited type
typedef struct View
{
	//TODO fix this!
	char id[8+1]; //as per chosen key size plus null terminator

	int width;
	int height;
	
	struct View * parent;
	
	Map childrenById; //for user convenience, and because builder is a runtime process
	List childrenByZ; //we render back to front of course, thus from end to start of this.
	
	uint64_t _childrenByIdKeys[VIEW_CHILDREN_MAX];
	struct View * _childrenById[VIEW_CHILDREN_MAX];
	struct View * _childrenByZ[VIEW_CHILDREN_MAX];
	
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
	struct Disjunction * disjunction; //in spite of typedef, use struct due to circular ref App->Disjunction TODO remove this ref, and allow Apps to send messages up to DJ?
	struct Map services;
	
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

typedef struct Disjunction
{
	struct Map apps;
	struct Map devices;
	
	struct App _apps[APPS_MAX];
	uint64_t _appKeys[APPS_MAX];
	
	struct Device _devices[DEVICES_MAX];
	uint64_t _deviceKeys[DEVICES_MAX];
	
	//struct Service _services[SERVICES_MAX];
	//Key _serviceKeys[SERVICES_MAX];

	struct Pointer pointer;
	//TODO Builder
	
	void * external; //anything we had to externally initialise / dispose of, but need a ref to inside App.
	//void (*start)(struct Disjunction * const this); 
	//void (*stop)(struct Disjunction * const this);
	void (*initialise)(struct Disjunction * const this);
	void (*dispose)(struct Disjunction * const this);
	
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
void Ctrl_start(Ctrl * const this);
void Ctrl_stop(Ctrl * const this);
void Ctrl_initialise(Ctrl * const this);
void Ctrl_update(Ctrl * const this);
void Ctrl_updatePost(Ctrl * const this);
void Ctrl_dispose(Ctrl * const this);
void Ctrl_disposeRecurse(Ctrl * const this);

void View_start(View * const this);
void View_stop(View * const this);
void View_construct(View * const this);
void View_initialise(View * const this);
void View_update(View * const this);
void View_disposeRecurse(View * const this);
bool View_isRoot(View * const this);
void View_addChild(View * const this, View * const child);
void View_onParentResizeRecurse(View * const this);
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
App * Disjunction_getApp(Disjunction * const this, const char * id);
//TODO...
void Disjunction_removeApp(Disjunction * const this, const char * id);
void Disjunction_addDevice(Disjunction * const this, const char * id, Device * const device);
void Disjunction_removeDevice(Disjunction * const this, const char * id);

void doNothing(void * const this);

Disjunction disjunction;// = {0}; //could use an empty?
#endif //DISJUNCTION_H