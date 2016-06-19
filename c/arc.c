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

static khiter_t k;

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
	strcat(parserFunctionName, "_fromConfig");
	LOGI("[ARC]    UpdaterComponent_initialise() (id=%s class=%s)\n", component->id, componentClassName);
	ParserFunction parser = addressofDynamic(parserFunctionName);
	
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

void UpdaterComponent_dispose(UpdaterComponent * component)
{
	//get parser if available
	const char * componentClassName = ezxml_attr(component->config, "class");
	char parserFunctionName[STRLEN_MAX];
	strcpy(parserFunctionName, componentClassName);
	strcat(parserFunctionName, "_dispose");
	LOGI("[ARC]    UpdaterComponent_initialise() (id=%s class=%s)\n", component->id, componentClassName);
	ParserFunction dispose = addressofDynamic(parserFunctionName);
	
	if (dispose)
		dispose(component);
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
	components->owner = updater;
	
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
		}
		
		this->dispose(this);
		this->initialised = false;
	}
	
	kh_destroy(StrPtr, this->components.byId);
	
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
	kv_init(node->children);
	//kv_init(node->active);
	//kv_init(node->stopped);
	//kv_init(node->suspended);
	
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
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_initialise(nodeChild, types, recurse);
		}
	}
}

void Node_destruct(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	char id[64]; strcpy(id, this->id);
	LOGI("[ARC]    Node_destruct... (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
	
	//Ctrl * ctrl = this->ctrl;
	//View * view = this->view;
		
	//DFS destruct...
	if (recurse)
	{
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_destruct(nodeChild, types, recurse);
		}
	}
	
	//kill more senior nodes/updaters last (on way back up tree)
	if (((uint8_t)types) & CTRL)
	{
		if (this->ctrl)
			Updater_destruct((Updater *) this->ctrl);
	}
	if (((uint8_t)types) & VIEW)
	{
		if (this->view)
			Updater_destruct((Updater *) this->view); //initialises all descendants too
	}
	
	kh_destroy(StrPtr, this->childrenById);
	
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_destruct    (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
}


void Node_setCtrl(Node * node, Ctrl * ctrl)
{
	node->ctrl = ctrl;
	ctrl->node = node;
}

void Node_setView(Node * node, View * view)
{
	node->view = view;
	view->node = node;
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
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_start(nodeChild, types, recurse);
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
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_stop(nodeChild, types, recurse);
		}
	}

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_stop    (id=%s)\n", this->id);
	#endif
}

void Node_suspend(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_stop... (id=%s)\n", this->id);
	#endif
	
	if (recurse)
	{
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_suspend(nodeChild, types, recurse);
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
	LOGI("[ARC] ...Node_stop    (id=%s)\n", this->id);
	#endif
}

void Node_resume(Node * const this, UpdaterTypes types, bool recurse)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_stop... (id=%s)\n", this->id);
	#endif

	if (recurse)
	{
		for (uint32_t i = 0; i < kv_size(this->children); ++i)
		{
			Node * nodeChild = kv_A(this->children, i);
			Node_suspend(nodeChild, types, recurse);
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
	LOGI("[ARC] ...Node_stop    (id=%s)\n", this->id);
	#endif
}

void Node_update(Node * node)//, UpdaterTypes type, bool recurse)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Node_update... (id=%s)\n", node->id);
	#endif
	
	//Updater * const updater;
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating Ctrl... \n");
	#endif
	
	//pre
	//if (type & CTRL)
	//{
		if (node->ctrl)
			Updater_update((Updater *) node->ctrl);
	//}
	//if (type & VIEW)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating View... \n");
	#endif
		if (node->view)
		{
			/*
			for (int i = 0; i < kv_size(node->view->children); ++i)
			{
				WidgetPart * part = node->view->parts.a[0];
				part->update(part);
			}
			*/
			
			Updater_update((Updater *) node->view);
			
		}
	//}
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Updating child Nodes of %s... \n", node->id);
	#endif
	//children
	//if (recurse)
	//{
		for (uint32_t i = 0; i < kv_size(node->children); ++i)
		{
			Node * nodeChild = kv_A(node->children, i);
			Node_update(nodeChild);//, type, recurse);
		}
	//}
	
	//post
	//if (type & CTRL)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    Post-Updating Ctrl... \n");
	#endif
		if (node->ctrl)
			Updater_updatePost((Updater *) node->ctrl);

	//}
	//if (type & VIEW)
	//{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC]    PostUpdating View... \n");
	#endif
		if (node->view)
			Updater_updatePost((Updater *) node->view);
	//}
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Node_update    (id=%s)\n", node->id);
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

void Node_claimAncestry(Node * const this, Node * const child)
{
	//LOGI("@@@@this=%s child=%s root=%s\n", this->id, child->id, this->root->id);
	child->parent = this;
	child->root = this->root;
}

Node * Node_add(Node * const this, Node * const child)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Node_add... (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	kv_push(Node *, this->children, child);
	Node_claimAncestry(this, child);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Node_add    (id=%s) (child id=%s)\n", this->id, child->id);
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
	//LOGI("child count=%d\n", kv_size(this->children));
	//search first only amongst children - do not DFS
	for (uint32_t i = 0; i < kv_size(this->children); ++i)
	{
		Node * child = kv_A(this->children, i); //NB! dispose in draw order
		//LOGI("child id=%s\n", child->id);
		if (strcmp(id, child->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...Node_find    (id=%s) (descendant id=%s)\n", this->id, id);
			#endif
			//LOGI("HIT!\n");
			return child;
		}
		else
			kv_push(Node *, candidatesAtNextDepth, child);
	}
	
	//BFS deeper if not found in children of this
	for (uint32_t i = 0; i < kv_size(candidatesAtNextDepth); ++i)
	{
		Node * child = kv_A(candidatesAtNextDepth, i);
		Node * result = Node_find(child, id);
		if (result) return result;
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("-[ARC] ...Node_find    (id=%s) (descendant id=%s)\n", this->id, id);
	#endif
	
	return NULL;
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

void View_onParentResize(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    View_onParentResize... (id=%s)\n", this->node->id);
	#endif
	
	this->onParentResize(this);
	
	for (uint32_t i = 0; i < kv_size(this->node->children); ++i)
	{
		Node * node = kv_A(this->node->children, i);
		View * child = node->view; //NB! dispose in draw order
		
		//depth first - update child and then recurse to its children
		if (child)
			View_onParentResize(child);
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

//-------- Builder -------//

typedef void * (*BuildFunction) (ezxml_t xml);

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

void Builder_component(ezxml_t componentXml, UpdaterComponents * components)
{
	const char * componentId = ezxml_attr(componentXml, "id");
	const char * componentClassName = ezxml_attr(componentXml, "class");
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_component... (id=%s class=%s)\n", componentId, componentClassName);
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
	LOGI("[ARC] ...Builder_component    (id=%s class=%s)\n", componentId, componentClassName);
	#endif// ARC_DEBUG_ONEOFFS
}

void Builder_components(ezxml_t xml, UpdaterComponents * components)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_components...\n");
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
				Builder_component(elementXmlCopy, components);

				elementXmlCopy = elementXmlCopy->next;
			}
		}
	}
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_components   \n");
	#endif// ARC_DEBUG_ONEOFFS
}

View * Builder_view(Node * node, ezxml_t viewXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_view...\n");
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
	
	//parent-child chain
	Node_setView(node, view); //must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	
	//get type names used for reflection in UpdaterComponent data path drilldown, then build UpdaterComponents
	view->ownClassName = (char *) viewClass;
	Builder_components(ezxml_child(viewXml, "components"), &view->components);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_view   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return view;
}

Ctrl * Builder_ctrl(Node * node, ezxml_t ctrlXml)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_ctrl...\n");
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
	
	//parent-child chain
	Node_setCtrl(node, ctrl); //must be done here *before* further attachments, so as to provide full ancestry (incl. app & hub) to descendants
	
	//get type names used for reflection in UpdaterComponent data path drilldown, then build UpdaterComponents
	ctrl->ownClassName = (char *) ctrlClass;
	Builder_components(ezxml_child(ctrlXml, "components"), &ctrl->components);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_ctrl   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return ctrl;
}

/// Build the config-specified contents into an already-constructed node.
Node * Builder_nodeContents(Node * const node, Node * const parentNode, ezxml_t nodeXml)
{
	const char * id = ezxml_attr(nodeXml, "id");
	if (id)
		strcpy(node->id, id);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_nodeContents... (id=%s)\n", id);
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
			Builder_view(node, viewXml);
		//Node_setView(node, view);
	}
	
	//ctrl
	ezxml_t ctrlXml = ezxml_child(nodeXml, "ctrl");
	if (ctrlXml)
	{
		//Ctrl * ctrl = 
			Builder_ctrl(node, ctrlXml);
		//Node_setCtrl(node, ctrl);
	}
	
	ezxml_t nodesXml = ezxml_child(nodeXml, "nodes");
	for (ezxml_t childNodeXml = ezxml_child(nodesXml, "node"); childNodeXml; childNodeXml = childNodeXml->next)
	{
		const char * childId = ezxml_attr(childNodeXml, "id");
		Node * childNode = Node_construct(childId);
		childNode = Builder_nodeContents(childNode, node, childNodeXml);
		
		Node_add(node, childNode);
		
		//TODO - to prevent having to pass (parent) node into this function
		//(read initial node from config - to be contained therein)
		//childNode = Builder_nodeContents(childNodeXml)
		//if (!childNode->model)
		//	childNode->model = node->model;
		//Builder_components(ctrlXml, &childNode->ctrl->components);
		//Builder_components(viewXml, &childNode->view->components);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_nodeContents   (id=%s)\n", id);
	#endif// ARC_DEBUG_ONEOFFS
	
	return node;
}

static ezxml_t rootNodeXml;

Node * Builder_nodeFromFilename(const char * configFilename)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC]    Builder_buildFromConfig...\n");
	#endif// ARC_DEBUG_ONEOFFS
	
	rootNodeXml = ezxml_parse_file(configFilename);
	
	const char * id = ezxml_attr(rootNodeXml, "id");
	Node * rootNode = Node_construct(id);
	Builder_nodeContents(rootNode, NULL, rootNodeXml);

	//ezxml_free(rootNodeXml);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Builder_buildFromConfig   \n");
	#endif// ARC_DEBUG_ONEOFFS
	
	return rootNode;
}

void Builder_dispose()
{
	ezxml_free(rootNodeXml);
}

//--------- misc ---------//
void Updater_doNothing(Updater * const this){/*LOGI("[ARC] Updater_doNothing\n");*/}
void View_doNothing(View * const this){/*LOGI("[ARC] View_doNothing\n");*/}
bool View_doNothing_return_bool(View * const this){return false;/*LOGI("[ARC] View_doNothing\n");*/}
bool True(){return true;}
bool False(){return false;}
