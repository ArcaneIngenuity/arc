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

//@file arc.h

/*! \mainpage Main Page
Arc is a lightweight applications development framework for realtime apps, specifically geared for games, simulations, and rich user experiences. See [README](md__r_e_a_d_m_e.html "README") for more. 

Source is available [here](https://github.com/ArcaneIngenuity/arc "arc"). 
 */
#ifndef ARC_H
#define ARC_H

#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "klib/kvec.h"
#include "klib/khash.h"
#include "ezxml/ezxml.h"
#include "../log/log.h"

//#define SERVICES_MAX 16
#define APPS_MAX 4
#define DEVICES_MAX 16
#define STRLEN_MAX 64

// setup khash for key/value types
// shorthand way to get the key from hashtable or defVal if not found
#define kh_get_val(kname, hash, key, defVal) ({k=kh_get(kname, hash, key);(k!=kh_end(hash)?kh_val(hash,k):defVal);})

// shorthand way to set value in hash with single line command.  Returns value
// returns 0=replaced existing item, 1=bucket empty (new key), 2-adding element previously deleted
#define kh_set(kname, hash, key, val) ({int ret; k = kh_put(kname, hash,key,&ret); kh_value(hash,k) = val; ret;})

#ifndef KH_DECL_STRPTR
#define KH_DECL_STRPTR
static const int StrPtr = 36;
KHASH_DECLARE(StrPtr, kh_cstr_t, uintptr_t)
#endif//KH_DECL_STRPTR

/// Describes a method on some object instance (usually a View) that handles a published event. 
typedef void (*SubHandler)(void * this, void * event);

/// A Sub(scriber) to some Pub(lisher).

/// Wraps the subscribing instance, coupled with its SubHandler-conformant method, into a single structure for Pub(lisher) use.
typedef struct Sub //INTERNAL USE ONLY
{
	void * instance; ///< The object instance i.e. the actual subscriber.
	SubHandler handler; ///< The method to call on the instance, to handle a published event.
} Sub;

/// A Pub(lisher) that publishes to Sub(scriber)s.

/// One instance hereof handles one specific event type. For more event types, attach more Pubs to the same (sub)model.
typedef struct Pub
{
	char name[STRLEN_MAX]; ///< The name by which this Pub(lisher) is referenced (used as key).
	kvec_t(Sub) subsList; ///< The list of Sub(scriber)s to which this Pub(lisher) publishers events.
} Pub;

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
	char id[STRLEN_MAX]; ///< (unimplemented) ID by which a View is retrieved from its App->viewsById.
	struct Hub * hub; ///< \brief The Hub which owns and manages this View's owner App.
	struct App * app; ///< \brief The App which owns and manages this View.
	
	bool updating; ///< Should this have View_update() called on it every frame?
	bool initialised; ///< True after first initialise. If re- initialise is required, this should be manually reset to false.
	void * model; ///< The model associated with this View. May or may not be the same as this View's App's model, depending on \link Configuration \endlink.
	
	struct View * root;
	struct View * parent; ///< View's parent View, if any.
	kvec_t(struct View *) childrenByZ; ///< View's children, where index is Z-order (Z is the stacking/draw order, i.e. goes positive out of screen).
	//int childrenCount; ///< The number of child Views held by this parent View. Negative for invalid return values (e.g. on seek).
	
	int width; ///< Width of this View. (NEEDS REVIEW - should be float to accommodate any user units; perhaps should be in user subclass of View)
	int height; ///< Height of this View. (NEEDS REVIEW - should be float to accommodate any user units; perhaps should be in user subclass of View)

	void (*start)(struct View * const this); ///< \brief User-supplied callback for when this View start()s.
	void (*stop)(struct View * const this); ///< \brief User-supplied callback for when this View stop()s.
	void (*suspend)(struct View * const this); ///< \brief User-supplied callback for when this View must suspend() due to a loss of rendering context.
	void (*resume)(struct View * const this); ///< \brief User-supplied callback for when this View must resume() due to regaining rendering context.
	void (*initialise)(struct View * const this); ///< \brief User-supplied callback for when this View initialise()s.
	void (*dispose)(struct View * const this); ///< \brief User-supplied callback for when this View dispose()s of its resources.
	void (*update)(struct View * const this); ///< \brief User-supplied callback for when this View update()s, i.e. update this View before its children update.
	void (*updatePost)(struct View * const this); ///< \brief User-supplied callback for when this View updatePost()s, i.e. update this View after its children update.
	void (*onParentResize)(struct View * const this); ///< \brief User-supplied callback for when this View's parent is resized. Root View resize is handled by some external (platform-specific) callback.
	//void (*enable)(struct View * const this); //start
	//void (*disable)(struct View * const this); //stop
} View;

/// Handles specific game / business / simulation logic within an App.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// A Ctrl instance may then be added to an App by setting App.ctrl; this will be the root Ctrl. Often, this will be the only Ctrl needed; though in some cases the user may prefer to use sub-%Ctrl%s attached to root.<br>
/// Sub-%Ctrl%s may be accessed through the App's root Ctrl.<br>
/// A Ctrl operates on a model. In arc, models need no specific type; they can be anything (and are internally denoted as void *). Ctrl writes new model state by reading extant model state, inputs and View state.
typedef struct Ctrl
{
	char id[STRLEN_MAX]; ///< ID used for Arc debugging mode.
	struct Hub * hub; ///< \brief The Hub which owns and manages this Ctrl's owner App.
	struct App * app; ///< \brief The App which owns and manages this Ctrl.
	
	bool updating; ///< Should this have Ctrl_update() called on it every frame?
	bool initialised; ///< True after first initialisation. If re-initialisation is required, manually reset this to false.
	void * model; ///< The model associated with this View. May or may not be the same as this View's App's (complete) model, depending on \link Configuration \endlink.
	
	void (*mustStart)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStart().
	void (*mustStop)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStop().
	void (*start)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl start()s.
	void (*stop)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl stop()s.
	void (*suspend)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl must suspend() due to a loss of rendering context.
	void (*resume)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl must resume() due to regaining rendering context.
	void (*initialise)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl initialise()s.
	void (*dispose)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl dispose()s of its resources.
	void (*update)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl update()s, i.e. update this Ctrl before any of its App's View%s update.
	void (*updatePost)(struct Ctrl * const this); ///< \brief User-supplied callback for when this Ctrl updatePost()s, i.e. update this Ctrl after any of its App's View%s update.
} Ctrl;

/// An application that consists of model, View%s and Ctrl%s; resides within a global application Hub.

/// Often, only a single App will be required within the (singleton)Hub.
/// If an App is to be run less frequently than specified by the rate dictated by its Hub, this can be handled in App_update by only updating full update logic when some accumulator reaches a certain amount of elapsed time or frames.
typedef struct App
{
	char id[STRLEN_MAX]; ///< \brief ID by which an App may be retrieved from its Hub (TODO); irrelevant except where updating multiple apps through the same Hub. (NEEDS REVIEW, apps go into indexed slots)
	struct Hub * hub; ///< \brief The Hub which owns and manages this App.
	//in spite of typedef, use struct due to circular ref App->Hub TODO remove this ref, and allow Apps to send messages up to DJ?
	//struct Map services;
	
	bool updating; ///< Should this have App_update() called on it every frame?
	bool initialised; ///< True after first initialisation. If re-initialisation is required, manually reset this to false.
	void * model; ///< The model associated with this App.
	struct View * view; ///< The root View associated with this App.
	struct Ctrl * ctrl; ///< The root Ctrl associated with this App.
	
	void (*initialise)(struct App * const this); ///< \brief User-supplied callback for when this App initialise()s.
	void (*dispose)(struct App * const this); ///< \brief User-supplied callback for when this App dispose()s of its resources.
	
	khash_t(StrPtr) * pubsByName;
} App;

/// Acts as a hybrid View / Ctrl or proxy between View and Ctrl aspects of an App, or as an interface to external mechanisms / event loops.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// When considering implementing a Service, bear in mind that their ideal is use is to tie into external mechanisms that don't obey the same timing as Hub_update (and thus App_update). A less desirable (but often necessary) use of Service%s is where View and Ctrl logic cannot be cleanly separated; for example, where an external library conflates these two aspects, or where user code cannot reasonably decouple the two, such as vertex data being used both for collision (Ctrl) and rendering (View) logic.
/// Services are accessed by their Service.id within App.
typedef struct Service
{
	struct App * app;
	void * models;
} Service;

/// A central point from which all App%s may be managed / updated.

/// A single timing mechanism (loop or callback) is used to run a Hub, from which each App's \link App_update \endlink will be called via Hub_update. This provides implicit synchronisation and best performance across multiple App%s that may be updating side-by-side.
/// As such, Hub needs no start / stop like App; Hub_update is run perpetually until the executable terminates.
typedef struct Hub
{
	App * apps[APPS_MAX]; //malloc'd array of pointers to apps -- allows ad-hoc allocation or batched pre-allocation
	int appsCount; ///< The number of valid App%s (counted from zero) currently in the Hub.apps array. (NEEDS REVIEW - these should act as slots and all should be checked, so don't need this)
	
	//TODO global Services?
	//struct Service _services[SERVICES_MAX];
	//Key _serviceKeys[SERVICES_MAX];

	//TODO Builder
	void (*initialise)(struct Hub * const this); ///< \brief User-supplied callback for when this Hub initialise()s.
	void (*dispose)(struct Hub * const this); ///< \brief User-supplied callback for when this Hub dispose()s of its resources.
	void (*suspend)(struct Hub * const this); ///< \brief User-supplied callback for when this Ctrl must suspend() due to a loss of rendering context.
	void (*resume)(struct Hub * const this); ///< \brief User-supplied callback for when this Hub must resume() due to regaining rendering context.
	
	void * external; ///< User-defined reference to global state; useful if avoiding global variables.

} Hub;

//FINAL/BASE METHODS

Ctrl * 		Ctrl_construct(const char * id, size_t sizeofSubclass); ///< \memberof Ctrl Constructs the Ctrl and sets all callbacks to do nothing.
void 		Ctrl_destruct(Ctrl * const this); ///< \memberof Ctrl Disposes of the Ctrl using \link dispose \endlink.
void 		Ctrl_setDefaultCallbacks(Ctrl * const this); ///< \memberof Ctrl Sets all Ctrl's callbacks to do nothing.
//bool 		Ctrl_mustStart(Ctrl * const this); ///< \memberof Ctrl
//bool 		Ctrl_mustStop(Ctrl * const this); ///< \memberof Ctrl
void 		Ctrl_start(Ctrl * const this); ///< \memberof Ctrl Starts the Ctrl using \link start \endlink.
void 		Ctrl_stop(Ctrl * const this); ///< \memberof Ctrl Stops the Ctrl using \link stop \endlink.
void		Ctrl_initialise(Ctrl * const this); ///< \memberof Ctrl Initialises the Ctrl using \link initialise \endlink.
void 		Ctrl_update(Ctrl * const this); ///< \memberof Ctrl Updates the Ctrl using \link update \endlink.
void 		Ctrl_updatePost(Ctrl * const this); ///< \memberof Ctrl Post-updates the Ctrl using \link updatePost \endlink.
void 		Ctrl_createPub(Ctrl * this, const char * name); ///< \memberof Ctrl Convenience method for creating a Pub(lisher) on this Ctrl's associated App.

View * 		View_construct(const char * id, size_t sizeofSubclass); ///< \memberof View Constructs the View and sets all callbacks to do nothing.
void 		View_destruct(View * const this); ///< \memberof View Disposes of the View and its children, depth-first, using \link dispose \endlink.
void 		View_setDefaultCallbacks(View * const this); ///< \memberof View Sets all View's callbacks to do nothing.
void 		View_start(View * const this); ///< \memberof View Starts the View using \link start \endlink.
void 		View_stop(View * const this); ///< \memberof View Stops the View using \link stop \endlink.
void 		View_suspend(View * const this); ///< \memberof View Has this View and its children %s \link suspend \endlink operations due to a loss of rendering context.
void 		View_resume(View * const this); ///< \memberof View Has this View and its children %s \link resume \endlink operations due to regaining rendering context.
void 		View_initialise(View * const this); ///< \memberof View Initialises the View using \link initialise \endlink.
void 		View_update(View * const this); ///< \memberof View Updates the View using \link update \endlink.
bool 		View_isRoot(View * const this); ///< \memberof View Is this the root View? (i.e. attached directly to \link App \endlink)
void 		View_onParentResize(View * const this); ///< \memberof View What to do when parent resizes, using \link onParentResize \endlink.
View * 		View_getChild(View * const this, char * id); ///< \memberof View Gets a child of this View by its \link id \endlink.
View *		View_addChild(View * const this, View * const child); ///< \memberof View Adds a child to this View, using its \link id \endlink.
//TODO... View * View_removeChild(View * const this, View * const child); //first get child by ID
//bool View_swapChildren(View * const this, int indexFrom, int indexTo);
void 		View_subscribe(View * this, const char * pubname, SubHandler handler); ///< \memberof View Convenience method for subscribing to a Pub(lisher) on this View's associated App.

App * 		App_construct(const char * id); ///< \memberof App Constructs the App and sets all callbacks to do nothing.
void 		App_destruct(App * const this); ///< \memberof App Destructs the App after disposing/destructing its Views and Ctrls, optionally using \link dispose \endlink. (NEEDS REVIEW, 2ND CLAUSE)
void 		App_initialise(App * const this); ///< \memberof App Initialises the App (and its attached Views and Ctrls, recursively) using \link initialise \endlink.
void 		App_update(App * const this); ///< \memberof App Updates the App using \link update \endlink.
void 		App_suspend(App * const this); ///< \memberof App Has the App's View%s and Ctrl%s \link suspend \endlink operations due to a loss of rendering context.
void 		App_resume(App * const this); ///< \memberof App Has the App's View%s and Ctrl%s \link resume \endlink operations due to regaining rendering context.
void 		App_start(App * const this); ///< \memberof App Starts the App using \link start \endlink.
void 		App_stop(App * const this); ///< \memberof App Stops the App using \link stop \endlink.
void 		App_setCtrl(App * app, Ctrl * ctrl); ///< \memberof App Sets the root Ctrl for the App, and reciprocally sets \link Ctrl.app \endlink and \link Ctrl.hub \endlink.
void 		App_setView(App * app, View * ciew); ///< \memberof App Sets the root View for the App, and reciprocally sets \link View.app \endlink and \link View.hub \endlink.
//TODO... void App_addService(App * const this, const char * id, Service * service);
//TODO... void App_removeService(App * const this, const char * id);

Hub * 		Hub_construct(); ///< \memberof Hub Constructs the Hub.
void 		Hub_destruct(Hub * const this); ///< \memberof Hub Destructs the Hub after \link App_destruct \endlink ing its App%s (e.g. OpenGL, OpenAL), optionally using \link dispose \endlink.
void 		Hub_initialise(Hub * const this); ///< \memberof Hub Initialises context that affects all App%s held by the Hub (e.g. OpenGL, OpenAL), using \link initialise \endlink.
void 		Hub_update(Hub * const this); ///< \memberof Hub Updates all Apps within the Hub.
void 		Hub_suspend(Hub * const this); ///< \memberof Hub Has all App%s \link suspend \endlink operations due to a loss of rendering context.
void 		Hub_resume(Hub * const this); ///< \memberof Hub Has all App%s \link resume \endlink operations due to regaining rendering context.
App * const Hub_addApp(Hub * const this, App * app); ///< \memberof Hub Adds an App to the Hub (at the next available slot, if any), which must have a valid App.id by the time it is added.
App * const Hub_getApp(Hub * const this, const char * const id); ///< \memberof Hub Gets an App from the Hub by its App.id.
//TODO... void Hub_removeApp(Hub * const this, const char * id);
//TODO... void Hub_addDevice(Hub * const this, const char * id, Device * const device);
//TODO... void Hub_removeDevice(Hub * const this, const char * id);

void 		Builder_buildFromConfig(Hub * const hub, const char * configFilename); ///< Build the Hub contents from a config file; path should be relative to executable.

void doNothing(void * const this); ///< A null-pattern callback which is the default when no user-defined callback has yet been supplied (prevents null pointer crashes).

#endif //ARC_H
