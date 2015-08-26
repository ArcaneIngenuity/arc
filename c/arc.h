//@file arc.h

/*! \mainpage Main Page
Arc is a lightweight applications development framework for realtime apps, specifically geared for games, simulations, and rich user experiences. See [README](md__r_e_a_d_m_e.html "README") for more. 

 */
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


/// A View with specific output (and possibly input) functionality in an App.

/// For every discrete View needed, allocate it and set the appropriate callbacks.<br>
/// When implementing a View this way, remember that it should be monolithic and represent a conceptual viewport that may be activated / deactivated, rather than every instance of something visible. Examples: in a game, a View might be the first person renderer rather than every instance of a game character; in a UI application, a View is better suited to representing a specific UI panel rather than each of the many controls within that Panel.<br>
/// A View instance may then be added to an App by setting App.view; this will be the root View. Further View%s should then be added as children of the root View, and so on, creating a View tree.<br>
/// Views may be accessed either by their View.id (which is set on construction) or zero-based depth/Z index.<br>
/// A View reads from a model in order to update its state. See Ctrl's description for more on models.
///
typedef struct View
{
	//TODO fix this!
	char * id; ///< (unimplemented) ID by which a View is retrieved from its App->viewsById.
	
	int width; 
	int height;
	
	struct View * parent; ///< View's parent View, if any.
	struct View * childrenByZ[VIEW_CHILDREN_MAX]; ///< View's children, where index is Z-order (Z is the stacking/draw order, i.e. goes positive out of screen).
	int childrenCount; ///< The number of child Views held by this parent View. Negative for invalid return values (e.g. on seek).
	
	bool running; ///< Should this have View_update() called on it every frame?
	bool initialised; ///< True after first initialisation. If re-initialisation is required, manually reset this to false.
	void * model; ///< The model associated with this View. May or may not be the same as this View's App's model, depending on \link Configuration \endlink.
	
	void (*start)(struct View * const this); ///< \brief User-supplied callback for when this View start()s.
	void (*stop)(struct View * const this); ///< \brief User-supplied callback for when this View stop()s.
	void (*initialise)(struct View * const this); ///< \brief User-supplied callback for when this View initialise()s.
	void (*dispose)(struct View * const this); ///< \brief User-supplied callback for when this View dispose()s of its resources.
	void (*update)(struct View * const this); ///< \brief User-supplied callback for when this View update()s.
	void (*updatePost)(struct View * const this); ///< \brief User-supplied callback for when this View updatePost()s.
	void (*onParentResize)(struct View * const this); ///< \brief User-supplied callback for when this View's parent is resized. Root View resize is handled by some external (platform-specific) callback.
	//void (*enable)(struct View * const this); //start
	//void (*disable)(struct View * const this); //stop
	//bool running; //start/stop
} View;
const struct View viewEmpty; ///< Used to set instance to empty / zero all its members, as a convenience to be used instead of memset(.., 0, ..).

/// Handles specific game / business / simulation logic within an App.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// A Ctrl instance may then be added to an App by setting App.ctrl; this will be the root Ctrl. Often, this will be the only Ctrl needed; though in some cases the user may prefer to use sub-Ctrl s attached to root.<br>
/// Sub-Ctrls may be accessed through the App's root Ctrl.<br>
/// A Ctrl operates on a model. In arc, models need no specific type; they can be anything (and are internally denoted as void *). Ctrl reads and writes model state based on input and View state.
typedef struct Ctrl
{
	bool running; ///< Should this have Ctrl_update() called on it every frame?
	bool initialised; ///< True after first initialisation. If re-initialisation is required, manually reset this to false.
	void * model; ///< The model associated with this View. May or may not be the same as this View's App's (complete) model, depending on \link Configuration \endlink.
	
	void (*mustStart)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStart().
	void (*mustStop)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStop().
	void (*start)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl start()s.
	void (*stop)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl stop()s.
	void (*initialise)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl initialise()s.
	void (*dispose)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl dispose()s of its resources.
	void (*update)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl update()s.
	void (*updatePost)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl updatePost()s.
} Ctrl;
const struct Ctrl ctrlEmpty; ///< Used to set instance to empty / zero all its members, as a convenience to be used instead of memset(.., 0, ..).

/// An application that consists of model, View%s and Ctrl%s; resides within a global application Hub.

/// Often, only a single App will be required within the (singleton)Hub.
/// If an App is to be run less frequently than specified by the rate dictated by its Hub, this can be handled in App_update by only running full update logic when some accumulator reaches a certain amount of elapsed time or frames.
typedef struct App
{
	char * id; ///< ID by which an App may be retrieved from its Hub (TODO); irrelevant except where running multiple apps through the same Hub. (NEEDS REVIEW, apps go into indexed slots)
	struct Hub * hub; //in spite of typedef, use struct due to circular ref App->Hub TODO remove this ref, and allow Apps to send messages up to DJ?
	//struct Map services;
	
	bool running; ///< Should this have App_update() called on it every frame?
	bool initialised; ///< True after first initialisation. If re-initialisation is required, manually reset this to false.
	void * model; ///< The model associated with this App.
	struct View * view; ///< The root View associated with this App.
	struct Ctrl * ctrl; ///< The root Ctrl associated with this App.
	
	void (*initialise)(struct App * const this); ///< \brief User-supplied callback for when this App initialise()s.
	void (*dispose)(struct App * const this); ///< \brief User-supplied callback for when this App dispose()s of its resources.
} App;
const struct App appEmpty; ///< Used to set instance to empty / zero all its members, as a convenience to be used instead of memset(.., 0, ..).

/// Acts as a hybrid View / Ctrl or proxy between View and Ctrl aspects of an App, or as an interface to external mechanisms / event loops.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// When considering implementing a Service, bear in mind that their ideal is use is to tie into external mechanisms that don't obey the same timing as Hub_update (and thus App_update). A less desirable (but often necessary) use of Service%s is where View and Ctrl logic cannot be cleanly separated; for example, where an external library conflates these two aspects, or where user code cannot reasonably decouple the two, such as vertex data being used both for collision (Ctrl) and rendering (View) logic.
/// Services are accessed by their Service.id within App.
typedef struct Service
{
	struct App * app;
	void * models;
} Service; 
const struct Service serviceEmpty; ///< Used to set instance to empty / zero all its members, as a convenience to be used instead of memset(.., 0, ..).

/// A central point from which all App%s may be managed / updated.

/// A single timing mechanism (loop or callback) is used to run a Hub, from which each App's \link App_update \endlink will be called via Hub_update. This provides implicit synchronisation and best performance across multiple App%s that may be running side-by-side.
/// As such, Hub needs no start / stop like App; Hub_update is run perpetually until the executable terminates.
typedef struct Hub
{
	App * apps[APPS_MAX]; //malloc'd array of pointers to apps -- allows ad-hoc allocation or batched pre-allocation
	int appsCount; ///< The number of valid App%s (counted from zero) currently in the Hub.apps array. (NEEDS REVIEW - these should act as slots and all should be checked, so don't need this)
	
	//struct Service _services[SERVICES_MAX];
	//Key _serviceKeys[SERVICES_MAX];

	//TODO Builder
	void (*initialise)(struct Hub * const this); ///< \brief User-supplied callback for when this Hub initialise()s.
	void (*dispose)(struct Hub * const this); ///< \brief User-supplied callback for when this Hub dispose()s of its resources.
	
	void * external; ///< User-defined reference to global state; useful if avoiding global variables.
	
} Hub;
const struct Hub hubEmpty; ///< Used to set instance to empty / zero all its members, as a convenience to be used instead of memset(.., 0, ..).

//FINAL/BASE METHODS

//bool 		Ctrl_mustStart(Ctrl * const this); ///< \memberof Ctrl
//bool 		Ctrl_mustStop(Ctrl * const this); ///< \memberof Ctrl
void 		Ctrl_start(Ctrl * const this); ///< \memberof Ctrl Starts the Ctrl using \link start \endlink.
void 		Ctrl_stop(Ctrl * const this); ///< \memberof Ctrl Stops the Ctrl using \link stop \endlink.
void		Ctrl_initialise(Ctrl * const this); ///< \memberof Ctrl Initialises the Ctrl using \link initialise \endlink.
void 		Ctrl_update(Ctrl * const this); ///< \memberof Ctrl Updates the Ctrl using \link update \endlink.
void 		Ctrl_updatePost(Ctrl * const this); ///< \memberof Ctrl \memberof View Post-updates the Ctrl using \link updatePost \endlink.
void 		Ctrl_dispose(Ctrl * const this); ///< \memberof Ctrl Disposes of the Ctrl using \link dispose \endlink.

void 		View_start(View * const this); ///< \memberof View Starts the View using \link start \endlink.
void 		View_stop(View * const this); ///< \memberof View Stops the View using \link stop \endlink.
void 		View_construct(View * const this); ///< \memberof View Constructs the View using \link construct \endlink. (NEEDS REVIEW, UNUSED?)
void 		View_initialise(View * const this); ///< \memberof View Initialises the View using \link initialise \endlink.
void 		View_update(View * const this); ///< \memberof View Updates the View using \link update \endlink.
void 		View_dispose(View * const this); ///< \memberof View Disposes of the View and its children, depth-first, using \link dispose \endlink.
bool 		View_isRoot(View * const this); ///< \memberof View Is this the root View? (i.e. attached directly to \link App \endlink)
void 		View_onParentResizeRecurse(View * const this); ///< \memberof View What to do when parent resizes, using \link onParentResize \endlink.
View * 		View_getChild(View * const this, char * id); ///< \memberof View Gets a child of this View by its \link id \endlink.
View *		View_addChild(View * const this, View * const child); ///< \memberof View Adds a child to this View, using its \link id \endlink.
//
//TODO... View * View_removeChild(View * const this, View * const child); //first get child by ID
//ArrayResult View_swapChildren(View * const this, int indexFrom, int indexTo);

void 		App_initialise(App * const this); ///< \memberof App Initialises the App using \link initialise \endlink.
void 		App_update(App * const this); ///< \memberof App Updates the App using \link update \endlink.
void 		App_start(App * const this); ///< \memberof App Starts the App using \link start \endlink.
void 		App_stop(App * const this); ///< \memberof App Stops the App using \link stop \endlink.
void 		App_dispose(App * const this); ///< \memberof App Disposes of the App and its Views and Ctrls, optionally using \link dispose \endlink. (NEEDS REVIEW, 2ND CLAUSE)
//TODO... void App_addService(App * const this, const char * id, Service * service);
//TODO... void App_removeService(App * const this, const char * id);

void 		Hub_construct(Hub * const this, int appsCount); ///< \memberof Hub Constructs the Hub using \link construct \endlink. (NEEDS REVIEW, UNUSED?)
void 		Hub_initialise(Hub * const this); ///< \memberof Hub Initialises context that affects all App s held by the Hub (e.g. OpenGL, OpenAL), using \link initialise \endlink.
void 		Hub_dispose(Hub * const this); ///< \memberof Hub Disposes of context that affects all App s held by the Hub (e.g. OpenGL, OpenAL), using \link dispose \endlink.
void 		Hub_update(Hub * const this); ///< \memberof Hub Updates all Apps within the Hub.
App * const Hub_addApp(Hub * const this, App * app); ///< \memberof Hub Adds an App to the Hub (at the next available slot, if any), which must have a valid App.id by the time it is added.
App * const Hub_getApp(Hub * const this, const char * const id); ///< \memberof Hub Gets an App from the Hub by its App.id.
//TODO... void Hub_removeApp(Hub * const this, const char * id);
//TODO... void Hub_addDevice(Hub * const this, const char * id, Device * const device);
//TODO... void Hub_removeDevice(Hub * const this, const char * id);

void doNothing(void * const this); ///< A null-pattern callback which is the default when no user-defined callback has yet been supplied (prevents null pointer crashes).

Hub hub;// = {0}; ///< Global instance of Hub. (NEEDS REVIEW, remove?)
#endif //ARC_H