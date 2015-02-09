#ifndef DISJUNCTION_H
#define DISJUNCTION_H

#define APPS_MAX 4
#define DEVICES_MAX 16
#define VIEW_CHILDREN_MAX 16
#include <stdbool.h>

#include "../../curtmap/src/curtmap.h"

typedef struct Timer
{
	//struct Disjunction * disjunction;

	float baseSec; /** internal */
	float deltaSec; /** public read / internal write */
	float accumulatorSec;
	float period;
	
	bool running;
	
	//Windows - QPC
	__int64 counter1;
	__int64 counter2;
	__int64 counterDelta;
	__int64 counterFrequency;
	double counterPeriod;
	
} Timer;

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
} Device;

//use as base struct for inherited type
typedef struct View
{
	struct View * parent;
	struct View * children[VIEW_CHILDREN_MAX];
	int childrenCount;
	bool initialised; //true after first start
	//bool running; //start/stop
	void * model;
	
	void (*start)(void * const this);
	void (*stop)(void * const this);
	void (*initialise)(void * const this);
	void (*dispose)(void * const this);
	void (*update)(void * const this);
	void (*updatePost)(void * const this);
	//void (*enable)(void * const this); //start
	//void (*disable)(void * const this); //stop
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
	struct CurtMap * services;
	
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
	struct CurtMap apps;
	struct CurtMap devices;
	//struct CurtMap services;
	
	bool initialised; //true after first start
	
	struct Timer * timer; //cannot know class / size
	
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
Timer * const Timer_constructor(float period);
int Timer_canConsume(Timer * const this);
void Timer_accumulate(Timer * const this);
void Timer_getDeltaSec(Timer * const this);
void Timer_consume(Timer * const this);
void Timer_start(Timer * const this);
void Timer_stop(Timer * const this);

void Device_constructor(Device * this, int channelCount);
void DeviceChannel_constructor(Device * this);

void Pointer_updateSelected(Pointer * const this);
void Pointer_hasChangedTarget(Pointer * const this);
void Pointer_hasChangedSelected(Pointer * const this);
void Pointer_hasMoved(Pointer * const this);

//bool Ctrl_mustStart(Ctrl * const this);
//bool Ctrl_mustStop(Ctrl * const this);
//void Ctrl_start(Ctrl * const this);
//void Ctrl_stop(Ctrl * const this);
//void Ctrl_initialise(Ctrl * const this);
void Ctrl_update(Ctrl * const this);
void Ctrl_updatePost(Ctrl * const this);
void Ctrl_dispose(Ctrl * const this);
void Ctrl_disposeRecurse(Ctrl * const this);

//void View_start(View * const this);
//void View_stop(View * const this);
//void View_dispose(View * const this);
//void View_update(View * const this);
//void View_updatePost(View * const this);
//void View_initialise(View * const this);
void View_updateRecurse(View * const this);
void View_disposeRecurse(View * const this);
bool View_isRoot(View * const this);
void View_addChild(View * const this, View * const child);
void View_swapChildren(View * const this, int indexFrom, int indexTo);

//void App_initialise(App * const this);
void App_update(App * const this);
void App_start(App * const this);
void App_stop(App * const this);
void App_dispose(App * const this);
void App_addService(App * const this, const char * id, Service * service);
void App_removeService(App * const this, const char * id);

//void Disjunction_initialise(Disjunction * const this);
void Disjunction_update(Disjunction * const this);
void Disjunction_start(Disjunction * const this);
void Disjunction_stop(Disjunction * const this);
void Disjunction_dispose(Disjunction * const this);
void Disjunction_addApp(Disjunction * const this, const char * id, App * const app);
void Disjunction_removeApp(Disjunction * const this, const char * id);
void Disjunction_addDevice(Disjunction * const this, const char * id, Device * const device);
void Disjunction_removeDevice(Disjunction * const this, const char * id);

void NullFunction(void * const this);

/*extern*/ Disjunction disjunction;// = {0}; //could use an empty?

#endif //DISJUNCTION_H