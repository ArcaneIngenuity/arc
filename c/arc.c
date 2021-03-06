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
#include "arctypes.h"

#define ARC_DEBUG_ONEOFFS 1
//#define ARC_DEBUG_UPDATES 1
//#define ARC_DEBUG_PUBSUB 1

//allow existing khash string key / pointer value defs to be used, else define here
#ifndef ARC_KHASH_TYPES_OFF
KHASH_DEFINE(StrPtr, 	kh_cstr_t, uintptr_t, kh_str_hash_func, kh_str_hash_equal, 1)
#endif//ARC_KHASH_TYPES_OFF

//-------- for *_configure() -------//

//typedef void * (*BuildFunction) (ezxml_t xml);

#define FOREACH_UPDATER_FUNCTION(instance, name, HANDLER) \
	HANDLER(instance, name, Updater_, start,) \
	HANDLER(instance, name, Updater_, stop,) \
	HANDLER(instance, name, Updater_, update,) \
	HANDLER(instance, name, Updater_, updatePost,) \
	HANDLER(instance, name, Updater_, initialise,) \
	HANDLER(instance, name, Updater_, dispose,) \
	HANDLER(instance, name, Updater_, suspend,) \
	HANDLER(instance, name, Updater_, resume,)

#define FOREACH_VIEW_FUNCTION(instance, name, HANDLER) \
	HANDLER(instance, name, View_, onParentResize,) \
	HANDLER(instance, name, View_, hasFocus, _return_bool)

#define GENERATE_ASSIGN_METHOD(instance, instancename, type, member, suffix) \
	name = ezxml_attr(instancename##Xml, #member); \
	if (name) instance member = addressofDynamic(name); \
	else \
	{ \
		strcpy(nameAssembled, instancename##Class); \
		strcat(nameAssembled, "_"); \
		strcat(nameAssembled, #member); \
		instance member = addressofDynamic(nameAssembled); \
	} \
	if (!instance member) \
	{ \
		instance member = & type##doNothing##suffix; \
		LOGI("[ARC]    Using default function: type##doNothing##suffix.\n"); \
	}

static khiter_t k;
static ezxml_t rootNodeXml;

//--------- Pub/Sub ---------//

/// Construct a Pub(lisher). This is usually called by Ctrl.initialise(), or possibly by Ctrl.update().
Pub * Pub_construct(const char * name)//, void * data)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Pub_construct (name=%name)\n", name);
	#endif
	Pub * pubPtr = calloc(1, sizeof(Pub));
	strcpy(pubPtr->name, name);
	//pubPtr->data = data;
	kv_init(pubPtr->subsList);
	return pubPtr;
}

/// Publish to the Sub(scriber) list.
void Pub_lish(Pub * pubPtr, void * info)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Pub_lish\n");
	#endif
	for (uint32_t i = 0; i < kv_size(pubPtr->subsList); ++i)
	{
		Sub * subPtr = &kv_A(pubPtr->subsList, i);
		subPtr->handler(subPtr->instance, info);
	}
}

/// Sub(scribe) to the Pub(lisher).

/// A Sub(scriber) knows what the data
void Sub_scribe(Sub * subPtr, Pub * pubPtr)
{
	#ifdef ARC_DEBUG_PUBSUB
	LOGI("[ARC]    Sub_scribe\n");
	#endif
	kv_push(Sub, pubPtr->subsList, *subPtr);
}

//---------UpdaterComponent-----------//

void UpdaterComponent_initialise(UpdaterComponent * component)
{
	
	//get parser if available
	const char * componentClassName = ezxml_attr(component->config, "class");
	char parserFunctionName[STRLEN_MAX];
	strcpy(parserFunctionName, componentClassName);
	strcat(parserFunctionName, "_configure");
	LOGI("[ARC]    UpdaterComponent_initialise() (id=%s class=%s)\n", component->id, componentClassName);
	ConfigureFunction parser = addressofDynamic(parserFunctionName);
	
	//Updater * element = component->group->owner;
	
	if (parser) //if component constructor is available
	{
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC]    Parser function found: %s :: %s.\n", componentClassName, parserFunctionName);
		#endif// ARC_DEBUG_ONEOFFS
		parser(component);
	}
	else //cannot construct component without function
	{
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC]    Parser function  not found: %s :: %s.\n", componentClassName, parserFunctionName);
		#endif// ARC_DEBUG_ONEOFFS
		exit(EXIT_FAILURE);
	}
}

void UpdaterComponent_configure(ezxml_t componentXml, UpdaterComponents * components)
{
	const char * componentId = ezxml_attr(componentXml, "id");
	const char * componentClassName = ezxml_attr(componentXml, "class");
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    UpdaterComponent_configure... (id=%s class=%s)\n", componentId, componentClassName);
	#endif// ARC_DEBUG_ONEOFFS
	
	UpdaterComponent * component = calloc(1, sizeofDynamic(componentClassName));
	strncpy(component->id, componentId, STRLEN_MAX);
	component->config = componentXml;
	component->group = components;
	//TODO this part (attachment) should happen after buildUpdaterComponent returns a valid UpdaterComponent *
	kv_push(UpdaterComponent *, components->ordered, component);
	kh_set(StrPtr, components->byId, component->id, (uintptr_t)component);
	
	component->runOnBuild = ezxml_attr(componentXml, "runOnBuild"); //don't need ="something", just need "runOnBuild"
	if (component->runOnBuild)
	{
		LOGI("[ARC]    Processing UpdaterComponent during build...\n");
		UpdaterComponent_initialise(component);
	}
	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...UpdaterComponent_configure    (id=%s class=%s)\n", componentId, componentClassName);
	#endif// ARC_DEBUG_ONEOFFS
}

void UpdaterComponent_dispose(UpdaterComponent * component)
{
	//get parser if available
	const char * componentClassName = ezxml_attr(component->config, "class");
	char parserFunctionName[STRLEN_MAX];
	strcpy(parserFunctionName, componentClassName);
	strcat(parserFunctionName, "_dispose");
	LOGI("[ARC]    UpdaterComponent_dispose() (id=%s class=%s)\n", component->id, componentClassName);
	ConfigureFunction dispose = addressofDynamic(parserFunctionName);
	
	if (dispose)
		dispose(component);
}

//--------- UpdaterComponents ---------//
void UpdaterComponents_configure(ezxml_t xml, UpdaterComponents * components)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    UpdaterComponents_configure...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	ezxml_t elementXml, elementXmlCopy;
	//first count the elements and resize the vec once-off - this prevents issues
	//with using the kvec's elements as keys into components.byId (due to realloc)
	size_t count = 0;
	for (elementXml = ezxml_child_any(xml); elementXml; elementXml = elementXml->ordered) //run through distinct child element names
	{
		if (strcmp(ezxml_name(elementXml), "component") == 0) 
			++count;
	}
	kv_resize(UpdaterComponent *, 	components->ordered, 	count);
	kh_resize(StrPtr,   	components->byId, 		count);

	//TODO find custom elements and build them using their name as a key into a map provided for each element type
	for (elementXml = ezxml_child_any(xml); elementXml; elementXml = elementXml->sibling) //run through distinct child element names
	{
		if (strcmp(ezxml_name(elementXml), "component") == 0) 
		{
			elementXmlCopy = elementXml;
			while (elementXmlCopy) //iterate over child elements of same name (that sit adjacent?)
			{
				UpdaterComponent_configure(elementXmlCopy, components);

				elementXmlCopy = elementXmlCopy->next;
			}
		}
	}
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...UpdaterComponents_configure   \n");
	#endif// ARC_DEBUG_ONEOFFS
}

//---------Updater-----------//

void Updater_resolveDataPath(void ** dataPtr, const char * dataClass, const char * dataPathString)
{
	void * data = *dataPtr; 
	//int c = 0;
	//int g = 0; //group count
	//const char * dataClassNew;
	
	kvec_t(DataPathElement) elements;
	kv_init(elements);
	
	if (dataPathString)
	{
		//prep the buffer in which we will have pointers to different membernames
		//necessary as we need to null-terminate each.
		char dataPath[strlen(dataPathString)];
		strcpy(dataPath, dataPathString);
		
		char * dataPathPtr = dataPath;
		
		DataPathElement element = {0};
		bool inMemberName = true;
		bool lastWasSep = true;

		//we must first get the full list of symbols, only then can we operate on them
		if (dataPathPtr[0] == '&')
		{
			LOGI("is address\n");
			
			element.symbol = 1;
			kv_push(DataPathElement, elements, element);
			memset(&element, 0, sizeof(element));
			lastWasSep = true;
			dataPathPtr[0] = '\0';
			++dataPathPtr;
			
		}
		
		while (dataPathPtr[0] != '\0') //seek char by char to end
		{
			if (dataPathPtr[0] == '.') //dot operator - offset to existing address
			{
				element.symbol = 2;
				kv_push(DataPathElement, elements, element);
				memset(&element, 0, sizeof(element));
				
				inMemberName = false;
				dataPathPtr[0] = '\0';
			}
			else
			{
				if (dataPathPtr[0] == '-' &&
					dataPathPtr[1] == '>') //deref operator - dereference / reset existing address to new
				{
					element.symbol = 3;
					kv_push(DataPathElement, elements, element);
					memset(&element, 0, sizeof(element));
					
					inMemberName = false;
					dataPathPtr[0] = dataPathPtr[1] = '\0';
					
					++dataPathPtr; //get past the extra character
				}
			}
		
			if (!inMemberName) //we just had a separator
			{
				//kv_push(DataPathElement, elements, element);
				memset(&element, 0, sizeof(element));
				inMemberName = true; //immediately assume next char begins a valid member name.
				lastWasSep = true;
			}
			else //in member name
			{
				if (lastWasSep)
				{
					//memset(&element, 0, sizeof(element));
					element.symbol = 0;
					element.memberName = dataPathPtr;
					lastWasSep = false;
					LOGI("!\n");
				}
			}
			
			++dataPathPtr;
		}
		kv_push(DataPathElement, elements, element); //push in the final element (should certainly be a member name)
		
		//void * address = data; //initial - start with the element itself
		//TODO the above only if we don't have a member name before first sep
		//LOGI("-data=%p\n", data);
		//we now have the full list of symbols - get final result
		DataPathElement elementLast;
		//bool reference = false;
		for (uint32_t i = 0; i < kv_size(elements); ++i)
		{
			element = kv_A(elements, i);
			//LOGI("symbol=%d\n", element.symbol);
			//LOGI("class=%s member=%s\n", dataClass, element.memberName);

			switch (element.symbol)
			{
				case Address:
					//reference = true;
					break;
				case Member:
					//LOGI("member: %s\n", element.memberName);
					
					if (elementLast.symbol == Offset)
					{
						size_t offset = offsetofDynamic(dataClass, element.memberName);
						//LOGI("offset to member=%u\n", offset);
						data += offset;
					}
					else if (elementLast.symbol == Deref)
					{
						const char * memberClass = typeofMemberDynamic(dataClass, element.memberName);
						size_t size = sizeofDynamic(memberClass);
						size_t offset = offsetofDynamic(dataClass, element.memberName);
						
						//work with individual bytes in the absence of compile-time types
						char * ptr = (char*)data; 
						ptr += offset;
						//LOGI("member size of=%u / offset to=%u\n", size, offset);
						//LOGI("deref to member of type %s\n", memberClass);
						//LOGI("ptr+offset=%p\n", ptr);
						char buffer[size];
						memcpy(buffer, ptr, size);
						data = ptr;
						//int b = *(int*)data;
						//LOGI("result: %i\n", b);
					}
					break;
				case Offset:
					break;
				case Deref:
					break;
				
			}
			
			LOGI("data=%p\n", data);
			elementLast = element;
		}
		//if (!reference)
		//	data = *data;
	}
	kv_destroy(elements);
	*dataPtr = data;
}

void Updater_construct(Updater * updater)
{
	updater->components.byId = kh_init(StrPtr);
	kv_init(updater->components.ordered);
}

/// Uses the UpdaterComponents created by Builder, at initialise time.
void Updater_initialise(struct Updater * const updater)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Updater_initialise...\n");
	#endif
	
	UpdaterComponents * components = &updater->components;
	components->updater = updater;
	
	//parse all components in order of declaration
	for (uint32_t i = 0; i < kv_size(components->ordered); ++i)
	{
		UpdaterComponent * component = kv_A(components->ordered, i);
		
		if (!component->runOnBuild)
		{
			LOGI("[ARC]    Processing UpdaterComponent during initialisation...\n");
			UpdaterComponent_initialise(component);
		}
	}
	
	//initialise element
	updater->initialise(updater);
	updater->initialised = true;
	LOGI("!");
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Updater_initialise\n");
	#endif
}

void Updater_destruct(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_destruct... \n");
	#endif
	
	if (this->initialised)
	{
		UpdaterComponents * components = &this->components;
		
		//delete all components in order of declaration
		for (uint32_t i = 0; i < kv_size(components->ordered); ++i)
		{
			UpdaterComponent * component = kv_A(components->ordered, i);
			UpdaterComponent_dispose(component);
			free(component);
		}
		
		this->dispose(this);
		this->initialised = false;
	}
	
	kh_destroy(StrPtr, this->components.byId);
	kv_destroy(this->components.ordered);
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Updater_destruct    \n");
	#endif
}

// DEFUNCT - builder auto-populates where appropriately named attribute not found!
void Updater_setDefaultCallbacks(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Updater_setDefaultCallbacks\n");
	#endif
	
	((Updater * const)this)->suspend 	= &Updater_doNothing;
	((Updater * const)this)->resume 	= &Updater_doNothing;
	((Updater * const)this)->initialise	= &Updater_doNothing;
	((Updater * const)this)->dispose 	= &Updater_doNothing;
	((Updater * const)this)->stop 		= &Updater_doNothing;
	((Updater * const)this)->update 	= &Updater_doNothing;
	((Updater * const)this)->updatePost	= &Updater_doNothing;
}


void Updater_start(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Updater_start... \n");
	#endif
	
	if (!this->updating)
	{
		this->start(this);
		this->updating = true;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Updater_start    \n");
	#endif
}

void Updater_stop(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Updater_stop... \n");
	#endif
	
	if (this->updating)
	{
		this->stop(this);
		this->updating = false;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Updater_stop    \n");
	#endif
}

void Updater_update(Updater * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updater_update... \n");
	#endif
	
	if (this->updating && !this->suspended)
		this->update(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Updater_update    \n");
	#endif
}

void Updater_updatePost(Updater * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updater_updatePost... \n");
	#endif
	
	if (this->updating && !this->suspended)
		this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Updater_updatePost    \n");
	#endif
}

void Updater_suspend(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Updater_suspend... \n");
	#endif
	
	if (!this->suspended)
	{
		this->suspend(this);
		this->suspended = true;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Updater_suspend    \n");
	#endif
}

void Updater_resume(Updater * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_resume... \n");
	#endif
	
	if (this->suspended)
	{
		this->resume(this);
		this->suspended = false;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_resume    \n");
	#endif
}

//--------- Ctrl ---------//

Ctrl * Ctrl_construct(size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_construct... \n");
	#endif
	
	//as we can't pass in a type, allocate size of the "subclass" - this is fine as "base"
	Ctrl * ctrl = calloc(1, sizeofSubclass);
	//Updater_setDefaultCallbacks(ctrl);
	Updater_construct((Updater *) ctrl);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_construct    \n");
	#endif
	
	return ctrl;
}

Ctrl * Ctrl_configure(Node * node, ezxml_t ctrlXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Ctrl_configure...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	const char * ctrlClass = ezxml_attr(ctrlXml, "class");
	
	size_t size = sizeofDynamic(ctrlClass);
	if (!size)
		exit(EXIT_FAILURE); //error message already output via sizeofDynamic()
	Ctrl * ctrl = Ctrl_construct(size);
	ctrl->config = ctrlXml;
	
	//drilldown to Ctrl's specific model, if any
	const char * name;
	char nameAssembled[STRLEN_MAX];
	FOREACH_UPDATER_FUNCTION(ctrl->, ctrl, GENERATE_ASSIGN_METHOD)
	
	//parent-child chain - must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	node->ctrl = ctrl;
	ctrl->node = node;
	
	//get type names used for reflection in UpdaterComponent data path drilldown, then build UpdaterComponents
	ctrl->ownClassName = (char *) ctrlClass;
	UpdaterComponents_configure(ezxml_child(ctrlXml, "components"), &ctrl->components);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_configure   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return ctrl;
}

void Ctrl_createPub(Ctrl * this, const char * name)
{
	Pub * pubPtr = Pub_construct(name);
	kh_set(StrPtr, this->pubsByName, name, (uintptr_t)pubPtr);
}

//--------- View ---------//

View * View_construct(size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_construct... \n");
	#endif
	
	//as we can't pass in a type, allocate size of the "subclass" - this is fine as "base"
	View * view = calloc(1, sizeofSubclass);
	view->onParentResize 	= &View_doNothing;
	view->hasFocus 			= &View_doNothing_return_bool;
	//Updater_setDefaultCallbacks(view);
	Updater_construct((Updater *) view);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_construct    \n");
	#endif
	
	return view;
}

View * View_configure(Node * node, ezxml_t viewXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_configure...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	const char * viewClass = ezxml_attr(viewXml, "class");
	
	size_t size = sizeofDynamic(viewClass);
	if (!size)
		exit(EXIT_FAILURE); //error message already output via sizeofDynamic()
	View * view = View_construct(size);
	view->config = viewXml;

	//function members
	const char * name;
	char nameAssembled[STRLEN_MAX];
	
	FOREACH_UPDATER_FUNCTION(view->,view,GENERATE_ASSIGN_METHOD)
	FOREACH_VIEW_FUNCTION	(view->,view,GENERATE_ASSIGN_METHOD)
	
	//parent-child chain - must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	node->view = view;
	view->node = node;
	
	//get type names used for reflection in UpdaterComponent data path drilldown, then build UpdaterComponents
	view->ownClassName = (char *) viewClass;
	UpdaterComponents_configure(ezxml_child(viewXml, "components"), &view->components);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_configure   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return view;
}

void View_onParentResize(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_onParentResize... (id=%s)\n", this->node->id);
	#endif
	
	this->onParentResize(this);
	
	Node * parent = this->node;
	Node * child = parent->childHead;
	while (child)
	{
		//NB! dispose in draw order
		
		//depth first - update child and then recurse to its children
		if (child->view)
			View_onParentResize(child->view);
		
		child = child->next;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_onParentResize    (id=%s)\n", this->node->id);
	#endif
}

void View_subscribe(View * this, const char * pubname, SubHandler handler)
{
	//TODO to reimplement, we'll need to pass the Ctrl in question in - don't rely on app
	/*
	App * app = this->app;
	
	//get the publisher in question off the app
	k = kh_get(StrPtr, app->pubsByName, pubname);
	Pub * pubPtr = kh_val(app->pubsByName, k);
	
	//add this and its handler to the Pub as a Sub (copy local scope struct data into subsList entry)
	Sub sub;
	sub.instance = this;
	sub.handler = handler;
	Sub_scribe(&sub, pubPtr);
	*/
}

void View_listen(View * const this)
{	
}

bool View_hasFocus(View * view)
{
	return view->hasFocus(view);
}

//--------- Node ---------//

Node * Node_construct(const char * id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_construct...(id=%s)\n", id);
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//Node * node __attribute__((cleanup (Node_destruct))) = malloc(sizeof(Node));
	//#else //no auto destructor!
	Node * node = calloc(1, sizeof(Node));
	//#endif//__GNUC__
	strcpy(node->id, id); //don't rely on pointers to strings that may be deallocated during runtime.
	node->childrenById = kh_init(StrPtr);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_construct   (id=%s)\n", id);
	#endif
	
	return node;
}

void Node_initialise(Node * const this, UpdaterTypes types, bool recurse)
{	
	//init more senior nodes/updaters first (on way down tree)
	if (this->ctrl)
		Updater_initialise((Updater *) this->ctrl);
	if (this->view)
		Updater_initialise((Updater *) this->view); //initialises all descendants too
	
	if (recurse)
	{
		Node * nodeChild = this->childHead;
		while (nodeChild)
		{
			Node_initialise(nodeChild, types, recurse);
			nodeChild = nodeChild->next;
		}
	}
}

/// Build the config-specified contents into an already-constructed node.
Node * Node_configureContents(Node * const node, Node * const parentNode, ezxml_t nodeXml)
{
	const char * id = ezxml_attr(nodeXml, "id");
	if (id)
		strcpy(node->id, id);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_configureContents... (id=%s)\n", id);
	#endif// ARC_DEBUG_ONEOFFS

	if (parentNode)
	{
		node->root = parentNode->root;
	}
	else //this is root
	{
		node->root = node;
	}
	node->config 		= nodeXml;
		
	//model
	ezxml_t modelXml = ezxml_child(nodeXml, "model");
	if (modelXml)
	{
		const char * modelClass = ezxml_attr(modelXml, "class");
		const char * modelPath = ezxml_attr(modelXml, "path");
		if (modelClass
			&& !modelPath) //DEV until we can forego specifying class on a sub-<model>
		{
			node->model = calloc(1, sizeofDynamic(modelClass));
			node->modelClassName =(char *) modelClass;
			//TODO - there is no node->modelClassName supplied for path, so children cannot drill down further.
			//TODO - ok, they can, but they need to assemble the whole string path from all ancestors to model class (root)
		}
		else if (modelPath)
		{
			if (parentNode)
			{
				node->model = parentNode->model;
				Updater_resolveDataPath(&node->model, parentNode->modelClassName, modelPath);
			}
		}
	}
	//don't use else here - there are un-elsed statements above so this acts as catchall
	if (!node->model && parentNode)
	{
		node->model = parentNode->model;
		node->modelClassName = parentNode->modelClassName;
	}
	
	//view
	ezxml_t viewXml = ezxml_child(nodeXml, "view");
	if (viewXml)
	{
		//View * view = 
			View_configure(node, viewXml);
		//Node_setView(node, view);
	}
	
	//ctrl
	ezxml_t ctrlXml = ezxml_child(nodeXml, "ctrl");
	if (ctrlXml)
	{
		//Ctrl * ctrl = 
			Ctrl_configure(node, ctrlXml);
		//Node_setCtrl(node, ctrl);
	}
	
	ezxml_t nodesXml = ezxml_child(nodeXml, "nodes");
	for (ezxml_t childNodeXml = ezxml_child(nodesXml, "node"); childNodeXml; childNodeXml = childNodeXml->next)
	{
		const char * childId = ezxml_attr(childNodeXml, "id");
		Node * childNode = Node_construct(childId);
		childNode = Node_configureContents(childNode, node, childNodeXml);
		
		Node_addChild(node, childNode);
		
		//TODO - to prevent having to pass (parent) node into this function
		//(read initial node from config - to be contained therein)
		//childNode = Node_configureContents(childNodeXml)
		//if (!childNode->model)
		//	childNode->model = node->model;
		//UpdaterComponents_configure(ctrlXml, &childNode->ctrl->components);
		//UpdaterComponents_configure(viewXml, &childNode->view->components);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_configureContents   (id=%s)\n", id);
	#endif// ARC_DEBUG_ONEOFFS
	
	return node;
}

Node * Node_configure(const char * configFilename)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_configure...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	rootNodeXml = ezxml_parse_file(configFilename);
	
	const char * id = ezxml_attr(rootNodeXml, "id");
	Node * rootNode = Node_construct(id);
	Node_configureContents(rootNode, NULL, rootNodeXml);

	//ezxml_free(rootNodeXml);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_configure   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return rootNode;
}

void Node_destruct(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC]    Node_destruct... (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
	
	//Ctrl * ctrl = this->ctrl;
	//View * view = this->view;
	
	bool isRoot = false;
	//must be done before parent of any given node is removed, below - so we can effectively check if root
	if (this->parent == NULL)
		isRoot = true;
	else
		Node_removeChild(this->parent, this); //removes all refs to this by parent or siblings
	
	//DFS destruct...
	if (recurse)
	{
		Node * child = this->childHead;
		while (child)
		{
			Node_destruct(child, types, recurse);
			child = child->next;
		}
	}
	
	//kill more senior nodes/updaters last (on way back up tree)
	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
		{
			Updater_destruct((Updater *) this->ctrl);
			this->ctrl = NULL; //prevent dangling pointer
		}
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
		{
			Updater_destruct((Updater *) this->view); //initialises all descendants too
			this->view = NULL; //prevent dangling pointer
		}
	}
	
	kh_destroy(StrPtr, this->childrenById);
	free(this); //dangling pointers handled by parent, in recurse loop above.
	if (isRoot) 
		ezxml_free(rootNodeXml);
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_destruct    (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
}

bool Node_isRoot(Node * const this)
{
	return this->parent == NULL;
}

void Node_start(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_start... (id=%s)\n", this->id);
	#endif

	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
		{
			Ctrl * ctrl = this->ctrl;
			Updater_start((Updater *) ctrl); //it is left to Ctrls to start Views
		}
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
		{
			View * view = this->view;
			Updater_start((Updater *) view); //it is left to Ctrls to start Views
		}
	}
	if (recurse)
	{
		Node * child = this->childHead;
		while (child)
		{
			Node_start(child, types, recurse);
			child = child->next;
		}
	}

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_start    (id=%s)\n", this->id);
	#endif
}


void Node_stop(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_stop... (id=%s)\n", this->id);
	#endif

	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
		{
			Ctrl * ctrl = this->ctrl;
			Updater_stop((Updater *) ctrl);
		}
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
		{
			View * view = this->view;
			Updater_stop((Updater *) view);
		}
	}
	if (recurse)
	{
		Node * child = this->childHead;
		while (child)
		{
			Node_stop(child, types, recurse);
			child = child->next;
		}
	}

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_stop    (id=%s)\n", this->id);
	#endif
}

void Node_suspend(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_suspend... (id=%s)\n", this->id);
	#endif
	
	if (recurse)
	{
		Node * child = this->childHead;
		while (child)
		{
			Node_suspend(child, types, recurse);
			child = child->next;
		}
	}
	
	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
		{
			Updater_suspend((Updater *) this->ctrl);
		}
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
		{
			Updater_suspend((Updater *) this->view);
		}
	}

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_suspend    (id=%s)\n", this->id);
	#endif
}

void Node_resume(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_resume... (id=%s)\n", this->id);
	#endif

	if (recurse)
	{
		Node * child = this->childHead;
		while (child)
		{
			Node_resume(child, types, recurse);
			child = child->next;
		}
	}
	
	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
		{
			Updater_resume((Updater *) this->ctrl);
		}
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
		{
			Updater_resume((Updater *) this->view);
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_resume    (id=%s)\n", this->id);
	#endif
}

void Node_update(Node * this)//, UpdaterTypes type, bool recurse)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Node_update... (id=%s)\n", this->id);
	#endif
	
	//Updater * const updater;
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating Ctrl... \n");
	#endif
	
	//pre
	//if (type & CTRL)
	//{
		if (this->ctrl)
			Updater_update((Updater *) this->ctrl);
	//}
	//if (type & VIEW)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating View... \n");
	#endif
		if (this->view)
		{
			/*
			for (int i = 0; i < kv_size(node->view->children); ++i)
			{
				WidgetPart * part = node->view->parts.a[0];
				part->update(part);
			}
			*/
			
			Updater_update((Updater *) this->view);
			
		}
	//}
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating child Nodes of %s... \n", this->id);
	#endif
	//children
	//if (recurse)
	//{
		Node * child = this->childHead;
		while (child)
		{
			Node_update(child);//, type, recurse);
			child = child->next;
		}
	//}
	
	//post
	//if (type & CTRL)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Post-Updating Ctrl... \n");
	#endif
		if (this->ctrl)
			Updater_updatePost((Updater *) this->ctrl);

	//}
	//if (type & VIEW)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    PostUpdating View... \n");
	#endif
		if (this->view)
			Updater_updatePost((Updater *) this->view);
	//}
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Node_update    (id=%s)\n", this->id);
	#endif
}

/*
void Node_update(Node * const this)
{
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	View * vRoot = this->view;
	View * vRootChild = vRoot->children.a[0]; //DEV - get by name
	
	if (vRootChild->type == WIDGET)
	{
		View * view = &vRootChild->children.a[0];
		if (widget->updateInput)
		{
			widget->updateInput(widget);
		}
		else
			LOGI("NO UPDATE.\n");
		
		// if (vRootUncast)
		// {
			// for (int i = 0; i < kv_size(vRootUncast); ++i)
			// {
				// kv_A(vRootUncast);

				
			// }
		// }
		
	}
	
	Ctrl_update(ctrl); //abstract
	//if (view != NULL) //JIC user turns off the root view by removing it (since this is the enable/disable mechanism)
	//really, we should just exit if either View or Ctrl are null, at App_start()
	if (((Updater *)view)->updating)
		View_update(view); //update whether group or widget
	Ctrl_updatePost(ctrl); //abstract
}
*/


/// Append a child to the tail end of the list of children, and set ancestry against parent.
Node * Node_addChild(Node * const this, Node * const child)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_addChild... (parent id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	//couple to parent
	//make findable by ID within parent in O(1) time.
	kh_set(StrPtr, this->childrenById, child->id, (uintptr_t) child);
	child->parent = this;
	child->root = this->root;
	
	//couple to siblings
	if (this->childTail)
		this->childTail->next = child;
	else //no children (head or tail or anything between) yet
		this->childHead = child;
	
	this->childTail = child;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_addChild    (parent id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	return child;
}

/// Removes the tail node from its parent.
Node * Node_removeChildTail(Node * const parent)
{
	Node * const child = parent->childTail;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_popFromParent... (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	if (child)
	{
		//decouple from parent
		kh_del(StrPtr, parent->childrenById, (uintptr_t)child->id);
		child->parent = NULL;	
		
		//decouple from siblings
		if (child->prev)
		{
			child->prev->next = NULL;
		}
		else //only child - because it is the tail without a prev
		{
			parent->childHead = NULL;
		}
		parent->childTail = child->prev; //whether NULL or not, set new tail as old tail's prev
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_popFromParent    (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	
	return child;
}

Node * Node_removeChild(Node * const parent, Node * const child)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_remove... (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	
	Node * result;
	
	if (parent)
	{
		//decouple from parent
		kh_del(StrPtr, parent->childrenById, (uintptr_t)child->id);
		child->parent = NULL;	

		//decouple from siblings
		if (child->prev)
			child->prev->next = child->next;
		else //is childHead
			parent->childHead = child->next;
		if (child->next)
			child->next->prev = child->prev;
		else //is childTail
			parent->childTail = child->prev;
			
		result = child;
	}
	else result = NULL; //for meaningful return

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_remove    (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	
	return result;
}

/// Removes a node from amonst its siblings (if any) and parent (if not root).
/// return null if it no removal took place (e.g. if we attempt to remove root).
Node * Node_orphan(Node * const child)
{
	Node * const parent = child->parent;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_orphan... (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	
	Node_removeChild(parent, child);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_orphan    (parent id=%s) (child id=%s)\n", parent->id, child->id);
	#endif
	
	return child;
}


Node * Node_find(Node * const this, const char * id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_find... (id=%s) (descendant id=%s)\n", this->id, id);
	#endif
	
	kvec_t(Node *) candidatesAtNextDepth;
	kv_init(candidatesAtNextDepth);
	//search first only amongst children - do not DFS
	//TODO correct this to use hash search at each descendant level, instead of linear search
	Node * child = this->childHead;
	
	while (child)
	{
		if (strcmp(id, child->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...Node_find    (id=%s) (descendant id=%s)\n", this->id, id);
			#endif
			
			kv_destroy(candidatesAtNextDepth);
			return child;
		}
		else
			kv_push(Node *, candidatesAtNextDepth, child);
		
		child = child->next;
	}
	
	//BFS deeper if not found in children of this
	//TODO use a temp linked list or array for this, to remove klib dep?
	for (uint32_t i = 0; i < kv_size(candidatesAtNextDepth); ++i)
	{
		Node * child = kv_A(candidatesAtNextDepth, i);
		Node * result = Node_find(child, id);
		if (result)
		{
			kv_destroy(candidatesAtNextDepth);
			return result;
		}			
	}
	
	kv_destroy(candidatesAtNextDepth);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_find    (id=%s) (descendant id=%s)\n", this->id, id);
	#endif
	
	return NULL;
}

//--------- misc ---------//
void Updater_doNothing(Updater * const this){/*LOGI("[ARC] Updater_doNothing\n");*/}
void View_doNothing(View * const this){/*LOGI("[ARC] View_doNothing\n");*/}
bool View_doNothing_return_bool(View * const this){return false;/*LOGI("[ARC] View_doNothing\n");*/}
bool True(){return true;}
bool False(){return false;}
