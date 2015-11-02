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
#include "arctypes.h"

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
typedef struct Sub
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

/// Value type for kvec_t(ArcString) (char array not possible).
typedef struct ArcString
{
	char name[STRLEN_MAX];
} ArcString;


/// Base class for a user-defined service that acts as a hybrid View / Ctrl or proxy between View and Ctrl aspects of an App, or as an interface to external mechanisms / event loops.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// When considering implementing a Service, bear in mind that their ideal is use is to tie into external mechanisms that don't obey the same timing as Hub_update (and thus App_update). A less desirable (but often necessary) use of Service%s is where View and Ctrl logic cannot be cleanly separated; for example, where an external library conflates these two aspects, or where user code cannot reasonably decouple the two, such as vertex data being used both for collision (Ctrl) and rendering (View) logic.
/// Services are accessed by their Service.id within App.
typedef struct Service
{
	struct App * app;
	void * models;
} Service;

typedef enum DataPathSymbol
{
	Member,
	Address,
	Offset,
	Deref
	
} DataPathSymbol;

/// One drilldown group comes after a single deref operator (or at beginning of chain).
typedef struct DataPathNode
{
	DataPathSymbol symbol; //0 = member, 1 = &, 2 = ., 3 = ->
	char * memberName; //use only if symbol == Member
	struct DataPathNode * next;
} DataPathNode;

struct Extensions;
struct Element;

/// Base class for a user-defined extension created from config.
typedef struct Extension
{
	char id[STRLEN_MAX]; ///< ID by which a user extension is retrieved from its owning View, Ctrl, App or Hub's ->extensions.byId.
	ezxml_t config;
	struct Extensions * group;
} Extension;

/// Collection of Extension-related information.
typedef struct Extensions
{
	kvec_t(Extension *) ordered; ///< Extensions included on this instance, in order of declaration in config. (must be list of pointers - each is allocated as a wider user type, see Builder_buildExtension() - so lost of Extension would truncate these).
	khash_t(StrPtr) * byId; ///< Extensions included on this instance, if using config.
	//kvec_t(ArcString) ids; ///< Array of fixed-length cstrings used as keys to extensions.byId (required once XML and its source strings are freed).
	struct Element * owner; ///< \internal Used to get a reference to the owning element when doing data path drilldown for Extensions. \endinternal
} Extensions;

/// A generic framework element, acts as base for VCAH which may be case hereto to use fields that way. 
typedef struct Element
{
	void * model; ///< The model associated with this instance; may depend on \link Configuration \endlink.
	
	bool initialised; ///< True after first initialise of the owner instance. If re- initialise is required, this should be manually reset to false. Always true for Hub.
	
	bool suspended; ///< (new, implement)
	
	void (*initialise)(struct Hub * const this); ///< \brief User-supplied callback for when owner instance initialise()s.
	void (*dispose)(struct Hub * const this); ///< \brief User-supplied callback for when owner instance dispose()s of its resources.
	void (*suspend)(struct Hub * const this); ///< \brief User-supplied callback for when owner instance must suspend() due to a loss of rendering context.
	void (*resume)(struct Hub * const this); ///< \brief User-supplied callback for when owner instance must resume() due to regaining rendering context.
	
	void * config; ///< Attachment point for config specific to this element, for possible implementation-specific parsing in Element subclasses.
	
	Extensions extensions; ///< Extensions owned by this View, if any.
	char * ownClassName; ///< \internal String class name of the owning Element's model; accesible to extensions in case of model datapath drilldown. \endinternal
	char * modelClassName; ///< \internal String class name of the owning Element; accesible to extensions in case of model datapath drilldown. \endinternal
} Element;

/// Base class for framework elements with custom update-related functions, i.e. Views and Ctrls.
typedef struct Updater
{
	Element base;
	
	bool updating; ///< Should this have update() called on it every frame?
	
	void (*start)(struct View * const this); ///< \brief User-supplied callback for when owner instance start()s.
	void (*stop)(struct View * const this); ///< \brief User-supplied callback for when owner instance stop()s.
	void (*update)(struct View * const this); ///< \brief User-supplied callback for when owner instance update()s, i.e. update this View before its children update.
	void (*updatePost)(struct View * const this); ///< \brief User-supplied callback for when owner instance updatePost()s, i.e. update this View after its children update.

} Updater;

/// Base class for a user-defined view that has specific rendering responsibilities.

/// For every discrete View needed, allocate it and set the appropriate callbacks.<br>
/// When implementing a View this way, remember that it should be monolithic and represent a conceptual viewport that may be activated / deactivated, rather than every instance of something visible. Examples: in a game, a View might be the first person renderer rather than every instance of a game character; in a UI application, a View is better suited to representing a specific UI panel rather than each of the many controls within that Panel.<br>
/// A View instance may then be added to an App by setting App.view; this will be the root View. Further View%s should then be added as children of the root View, and so on, creating a View tree.<br>
/// Views may be accessed either by their View.id (which is set on construction) or zero-based depth/Z index.<br>
/// A View reads from a model in order to update its state. See Ctrl's description for more on models.
///
typedef struct View
{
	Updater base;
	char id[STRLEN_MAX]; ///< (unimplemented) ID by which a View is retrieved from its parent view->childrenById.
	
	struct View * root; ///< View's root view, i.e. the View attached to the App.
	struct View * parent; ///< View's parent View, if not root.
	kvec_t(struct View *) children; ///< View's children, in order added (as config, if used); index may be used as Z-order.
	struct Hub * hub; ///< \brief The Hub which owns and manages this instance's owner App.
	struct App * app; ///< \brief The App which owns and manages this instance.
	
	void (*onParentResize)(struct View * const this); ///< \brief User-supplied callback for when this View's parent is resized. Root View resize is handled by some external (platform-specific) callback.
	//void (*enable)(struct View * const this); //start
	//void (*disable)(struct View * const this); //stop	
} View;

/// Base class for a user-defined controller that handles specific game / business / simulation logic within an App.

/// For every discrete Ctrl needed, allocate it and set the appropriate callbacks.<br>
/// A Ctrl instance may then be added to an App by setting App.ctrl; this will be the root Ctrl. Often, this will be the only Ctrl needed; though in some cases the user may prefer to use sub-%Ctrl%s attached to root.<br>
/// Sub-%Ctrl%s may be accessed through the App's root Ctrl.<br>
/// A Ctrl operates on a model. In arc, models need no specific type; they can be anything (and are internally denoted as void *). Ctrl writes new model state by reading extant model state, inputs and View state.
typedef struct Ctrl
{
	Updater base;
	char id[STRLEN_MAX]; ///< (unimplemented) ID by which a View is retrieved from its parent view->childrenById.
	
	struct Ctrl * root; ///< Ctrl's root view, i.e. the Ctrl attached to the App.
	struct Ctrl * parent; ///< Ctrl's parent Ctrl, if not root.
	kvec_t(struct Ctrl *) children; ///< Ctrl's children, in order added (as config, if used).
	struct Hub * hub; ///< \brief The Hub which owns and manages this instance's owner App.
	struct App * app; ///< \brief The App which owns and manages this instance.
	
	//void (*mustStart)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStart().
	//void (*mustStop)(struct Ctrl * const this); ///< \brief User-supplied callback for checking when this Ctrl mustStop().
} Ctrl;

/// An application that consists of model, View%s and Ctrl%s; resides within a global application Hub.

/// Often, only a single App will be required within the (singleton)Hub.
/// If an App is to be run less frequently than specified by the rate dictated by its Hub, this can be handled in App_update by only updating full update logic when some accumulator reaches a certain amount of elapsed time or frames.
typedef struct App
{
	Element base;
	char id[STRLEN_MAX]; ///< (unimplemented) ID by which a View is retrieved from its parent view->childrenById.
	
	bool updating;
	
	struct Hub * hub; ///< \brief The Hub which owns and manages this App.
	struct View * view; ///< The root View associated with this App.
	struct Ctrl * ctrl; ///< The root Ctrl associated with this App.
	
	khash_t(StrPtr) * pubsByName;
	
	//struct khash_t(StrPtr) services;
} App;


/// A central point from which all App%s may be managed / updated.

/// A single timing mechanism (loop or callback) is used to run a Hub, from which each App's \link App_update \endlink will be called via Hub_update. This provides implicit synchronisation and best performance across multiple App%s that may be updating side-by-side.
/// As such, Hub needs no start / stop like App; Hub_update is run perpetually until the executable terminates.
typedef struct Hub
{
	Element base;
	
	//TODO use kvec_t(App *)
	App * apps[APPS_MAX]; //malloc'd array of pointers to apps -- allows ad-hoc allocation or batched pre-allocation
	int appsCount; ///< The number of valid App%s (counted from zero) currently in the Hub.apps array. (NEEDS REVIEW - these should act as slots and all should be checked, so don't need this)
	
	void * external; ///< User-defined reference to global state; useful if avoiding global variables.
	
	//TODO Services?
	//TODO Builder
} Hub;

//FINAL/BASE METHODS

Ctrl * 		Ctrl_construct(const char * id, size_t sizeofSubclass); ///< \memberof Ctrl Constructs the Ctrl and sets all callbacks to do nothing.
void 		Ctrl_destruct(Ctrl * const this); ///< \memberof Ctrl Disposes of the Ctrl using \link dispose \endlink.
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
void 		Hub_dispose(Hub * const this); ///< \memberof Hub Disposes of context that affects all App%s held by the Hub (e.g. OpenGL, OpenAL), using \link initialise \endlink.
void 		Hub_update(Hub * const this); ///< \memberof Hub Updates all Apps within the Hub.
void 		Hub_suspend(Hub * const this); ///< \memberof Hub Has all App%s \link suspend \endlink operations due to a loss of rendering context.
void 		Hub_resume(Hub * const this); ///< \memberof Hub Has all App%s \link resume \endlink operations due to regaining rendering context.
App * const Hub_addApp(Hub * const this, App * app); ///< \memberof Hub Adds an App to the Hub (at the next available slot, if any), which must have a valid App.id by the time it is added.
App * const Hub_getApp(Hub * const this, const char * const id); ///< \memberof Hub Gets an App from the Hub by its App.id.
//TODO... void Hub_removeApp(Hub * const this, const char * id);
//TODO... void Hub_addDevice(Hub * const this, const char * id, Device * const device);
//TODO... void Hub_removeDevice(Hub * const this, const char * id);

void 		Builder_buildFromConfig(Hub * const hub, const char * configFilename); ///< Build the Hub contents from a config file; path should be relative to executable.
typedef void * (*ParserFunction)(Extension * extension);
void doNothing(void * const this); ///< A null-pattern callback which is the default when no user-defined callback has yet been supplied (prevents null pointer crashes).

#endif //ARC_H
