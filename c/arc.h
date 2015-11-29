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

struct UpdaterComponents;
struct Updater;
struct Node;


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


typedef enum DataPathElementSymbol
{
	Member,
	Address,
	Offset,
	Deref
	
} DataPathElementSymbol;

/// One drilldown group comes after a single deref operator (or at beginning of chain).
typedef struct DataPathElement
{
	DataPathElementSymbol symbol; //0 = member, 1 = &, 2 = ., 3 = ->
	char * memberName; //use only if symbol == Member
	struct DataPathElement * next;
} DataPathElement;

/// Base class for a user-defined component created from config.
typedef struct UpdaterComponent
{
	char id[STRLEN_MAX]; ///< ID by which a user component is retrieved from its owning View, Ctrl, App or Hub's ->components.byId.
	ezxml_t config;
	struct UpdaterComponents * group;
	bool runOnBuild;
} UpdaterComponent;

/// Collection of UpdaterComponent-related information.
typedef struct UpdaterComponents
{
	kvec_t(UpdaterComponent *) ordered; ///< UpdaterComponents included on this instance, in order of declaration in config. (must be list of pointers - each is allocated as a wider user type, see Builder_component() - so lost of UpdaterComponent would truncate these).
	khash_t(StrPtr) * byId; ///< UpdaterComponents included on this instance, if using config.
	//kvec_t(ArcString) ids; ///< Array of fixed-length cstrings used as keys to components.byId (required once XML and its source strings are freed).
	struct Updater * owner; ///< \internal Used to get a reference to the owning element when doing data path drilldown for UpdaterComponents. \endinternal
} UpdaterComponents;

/// Base class for framework elements with custom update-related functions, i.e. Views and Ctrls.
typedef struct Updater
{	
	//TODO make enum - 0=Stopped, 1=Suspended, 2=Updating
	bool updating; ///< Should this have update() called on it every frame?
	bool initialised; ///< True after first initialise of the owner instance. If re- initialise is required, manually reset this to false.
	bool suspended; ///< An alternative to suspending the whole containing Node, we can instead set this on an individual Updater.

	void * config; ///< Attachment point for config specific to this element, for possible implementation-specific parsing in Element subclasses.
	
	UpdaterComponents components; ///< UpdaterComponents owned by this instance, if any.
	char * ownClassName; ///< \internal String class name of the owning Element's model; accesible to components in case of model datapath drilldown. \endinternal
	
	struct Node * node; ///< Node associated with this Updater (could be up the chain of this type, Ctrl or View).
	
	void (*initialise)	(struct Updater * const this); ///< \brief Do this when owner Node initialise()s.
	void (*dispose)		(struct Updater * const this); ///< \brief Do this when owner Node dispose()s of its resources.
	void (*suspend)		(struct Updater * const this); ///< \brief Do this when owner Node must suspend() due to a loss of rendering context.
	void (*resume)		(struct Updater * const this); ///< \brief Do this when owner Node must resume() due to regaining rendering context.
	void (*start)		(struct Updater * const this); ///< \brief Do this when owner Node start()s.
	void (*stop)		(struct Updater * const this); ///< \brief Do this when owner Node stop()s.
	void (*update)		(struct Updater * const this); ///< \brief Do this when owner Node update()s, updates this before its children (on way down the tree).
	void (*updatePost)	(struct Updater * const this); ///< \brief Do this when owner Node updatePost()s, updates this after its children (on way up the tree).
} Updater;

typedef enum UpdaterTypes
{
	NONE, //0b00
	VIEW, //0b01
	CTRL, //0b10
	BOTH, //0b11
} UpdaterTypes;

typedef void (*UpdaterFunction)(struct Updater * const this);

/// Abstract class, extend to handle specific game / business / simulation logic within the containing Node by reading/writing part of the model.
typedef struct Ctrl
{
	struct Updater;
	
	khash_t(StrPtr) * pubsByName;
} Ctrl;

/// Abstract class, extend to handle specific game / business / simulation logic within the containing Node by reading/rendering model.
typedef struct View
{
	struct Updater;

	void (*onParentResize)	(struct View * const this); ///< \brief User-supplied callback for when this View's parent is resized. Root View resize is handled by some external (platform-specific) callback.
	bool (*hasFocus)		(struct View * const this); ///< Determine whether or not this View currently has focus.
	
	//void * spatial; ///> contains any spatial information specified by the user.
	
} View;

/// The basis for an App's structure; nodes create a tree.
typedef struct Node
{
	char id[STRLEN_MAX];
	
	struct Node * root;
	struct Node * parent;
	
	//TODO put into anonymous struct: this.children.inOrder vs this.children.byId
	khash_t(StrPtr) * childrenById;
	kvec_t(struct Node *) children; ///< Those child nodes which are processed on parent update.
	//kvec_t(Node *) active; ///< Those child nodes which are processed on parent update.
	//kvec_t(Node *) stopped;
	//kvec_t(Node *) suspended;
	
	char * modelClassName; ///< \internal String class name of the model; accesible to components in case of model datapath drilldown. \endinternal
	void * model; ///< The model associated with this instance; may depend on \link Configuration \endlink.
	struct View * view;
	struct Ctrl * ctrl;

	void * config;
	
	void * external;
} Node;

/// Used as the context object when we need to run a Node_*, Updater-related function as a single-arg callback.
typedef struct NodeUpdaterArgs //can't really contain the names of all six functions that use it!
{
	struct Node * this;
	enum UpdaterTypes types;
	bool recurse;
} NodeUpdaterArgs;

//FINAL/BASE METHODS
Node *		Node_construct		(const char * id);
void		Node_destruct		(Node * const node, UpdaterTypes type, bool recurse); //TODO should always recurse?
void		Node_start			(Node * const node, UpdaterTypes type, bool recurse);
void		Node_stop			(Node * const node, UpdaterTypes type, bool recurse);
void		Node_update			(Node * const node);//, UpdaterTypes type, bool recurse);
void		Node_suspend		(Node * const node, UpdaterTypes type, bool recurse); //TODO should always recurse
void		Node_resume			(Node * const node, UpdaterTypes type, bool recurse); //TODO "
void		Node_initialise		(Node * const node, UpdaterTypes type, bool recurse);
bool 		Node_isRoot  		(Node * const this); ///< \memberof View Is this the root View? (i.e. attached directly to \link App \endlink)
Node * 		Node_find			(Node * const this, const char * id); ///< \memberof View Gets a child of this View by its \link id \endlink.
Node *		Node_add			(Node * const this, Node * const child); ///< \memberof View Adds a child to this View, using its \link id \endlink.

void 		Updater_start		(Updater * const this); ///< \memberof Updater Starts the instance using \link start \endlink.
void 		Updater_stop		(Updater * const this); ///< \memberof Updater Stops the instance using \link stop \endlink.
void		Updater_initialise	(Updater * const this); ///< \memberof Updater Initialises the instance using \link initialise \endlink.
void		Updater_dispose		(Updater * const this); ///< \memberof Updater Disposes of the instance using \link dispose \endlink.
void 		Updater_update		(Updater * const this); ///< \memberof Updater Updates the instance using \link update \endlink.
void 		Updater_updatePost	(Updater * const this); ///< \memberof Updater Post-updates the instance using \link updatePost \endlink.
void 		Updater_suspend		(Updater * const this); ///< \memberof Updater \link suspend \endlink operations due to a loss of rendering context.
void 		Updater_resume		(Updater * const this); ///< \memberof Updater \link resume \endlink operations due to regaining rendering context.

Ctrl * 		Ctrl_construct		(size_t sizeofSubclass); ///< \memberof Ctrl Constructs the Ctrl and sets all callbacks to do nothing.
void 		Ctrl_destruct		(Ctrl * const this); ///< \memberof Ctrl Disposes of the Ctrl using \link dispose \endlink.
void 		Ctrl_createPub		(Ctrl * this, const char * name); ///< \memberof Ctrl Convenience method for creating a Pub(lisher) on this Ctrl's associated App.

View * 		View_construct		(size_t sizeofSubclass); ///< \memberof View Constructs the View and sets all callbacks to do nothing.
void 		View_destruct		(View * const this); ///< \memberof View Disposes of the View and its children, depth-first, using \link dispose \endlink.
void 		View_onParentResize	(View * const this); ///< \memberof View What to do when parent resizes, using \link onParentResize \endlink.
void 		View_subscribe		(View * this, const char * pubname, SubHandler handler); ///< \memberof View Convenience method for subscribing to a Pub(lisher) on this View's associated App.


void 		Builder_nodesByFilename(Node * const rootNode, const char * configFilename); ///< Build the Hub contents from a config file; path should be relative to executable.
typedef void * (*ParserFunction)(UpdaterComponent * component);

//misc
void doNothing(void * const this); ///< A null-pattern callback which is the default when no user-defined callback has yet been supplied (prevents null pointer crashes).
bool True();
bool False();

#endif //ARC_H