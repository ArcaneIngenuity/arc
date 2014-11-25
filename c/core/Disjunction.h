#define APPS_MAX 4
#define DEVICES_MAX 16
#include <stdbool.h>

typedef struct Timer
{
	struct Disjunction * disjunction;
	
	//void (* callback)(); /** abstract */
	//void (* update)(); /** final */
	//void (* getBaseSec)(); /** abstract */
	//void (* initialise)(); /** abstract */
	
	//void (* getDeltaSec)(); /** abstract */
	//void (* start)(); /** abstract */
	//void (* stop)(); /** abstract */

	float baseSec; /** internal */
	float deltaSec; /** public read / internal write */
	float accumulatorSec;
	float period;
	
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
/*
typedef struct DeviceHub
{
	Device devices[DEVICES_MAX];
} DeviceHub;
*/
typedef struct Pointer
{
} Pointer;


typedef struct App
{
	struct Disjunction * disjunction; //in spite of typedef, use struct due to circular ref App->Disjunction TODO remove this ref, and allow Apps to send messages up to DJ?
	//this.id = id; //for compound apps
	//this.services = new Disjunction.Core.ServiceHub(this);
	void * model; //cannot know class / size; struct YourModel must contain void * journals[]
	struct View * view; //cannot know class / size
	struct Ctrl * ctrl; //cannot know class / size
	//void * focus; //cannot know class / size
	
	//struct Ctrl  * ctrl;
	//struct View  * view;
	//struct View  * focus;
} App;

typedef struct Disjunction
{
	struct App * apps[APPS_MAX];  //in spite of typedef, use struct due to circular ref Disjunction->App
	//struct Timer * timer;
	//struct DeviceHub devices;
	struct Device * devices[DEVICES_MAX];
	
	struct Timer * timer; //cannot know class / size
	
	struct Pointer pointer;
	//services: undefined,
	//builder: undefined,
} Disjunction;

typedef struct Model
{
	
} Model;

typedef struct View
{

} View;

typedef struct Ctrl
{

} Ctrl;

Timer * const Timer_constructor(float period);
int Timer_canUpdate(Timer * const this);
void Timer_accumulate(Timer * const this);
void Timer_updateDeltaSec(Timer * const this);
void Timer_update(Timer * const this);
void Timer_start(Timer * const this);
void Timer_stop(Timer * const this);

void Device_constructor(Device * this, int channelCount);
/*
void DeviceHub_get(DeviceHub * this);
void DeviceHub_add(DeviceHub * this);
void DeviceHub_poll(DeviceHub * this);
void DeviceHub_flush(DeviceHub * this);
void DeviceHub_poll(DeviceHub * this);
*/
void Pointer_findTarget(Pointer * const this, void * const view);
void Pointer_updateSelected(Pointer * const this);
bool Ctrl_mustStart(Ctrl * const this);
bool Ctrl_mustStop(Ctrl * const this);
void Ctrl_start(Ctrl * const this);
void Ctrl_stop(Ctrl * const this);
void Ctrl_simulate(Ctrl * const this);
void View_start(Ctrl * const this);
void View_stop(Ctrl * const this);
void View_output(Ctrl * const this);
void App_update(App * const this);
void App_start(App * const this);
void App_dispose(App * const this);
void Disjunction_update(Disjunction * const this);
void Disjunction_loop(Disjunction * const this);
void Disjunction_start(Disjunction * const this);