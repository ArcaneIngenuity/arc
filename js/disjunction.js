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
/// \brief arc / disjunction - a realtime applications framework for native & web

//"use strict";

//FUNDAMENTALS
Disjunction = DJ = {Core: {}, Extensions: {}/*, Constants: {X:0, Y:1, Z:2}*/}; //namespace for class defs

disjunction = dj =
{
	WINDOW_CLASSES: true,
	WINDOW_CONSTANTS: true,
	PREFIXED_MARKUP: false,
	
	//constants
	constants:
	{
		X: 0,
		Y: 1,
		Z: 2
	},

	//instances
	apps: {}, //anything not in this map will not be updated by timer.
	timer:	undefined,
	devices: undefined,
	services: undefined,
	pointer: undefined,
	builder: undefined,
	
	
	//functions
	setPointer: function(deviceIndex, xChannelIndex, yChannelIndex, selectChannelIndex) //final
	{
		var pointer = this.pointer = new Disjunction.Core.Pointer();
		pointer.device = this.devices.array[deviceIndex];
		pointer.xChannel = pointer.device.channels[xChannelIndex];
		pointer.yChannel = pointer.device.channels[yChannelIndex];
		pointer.selectChannel = pointer.device.channels[selectChannelIndex];
	},
	
	update: function()
	{	
		this.devices.poll();
		
		for (var id in this.apps)
		{	
			var app = this.apps[id];
			app.update();
		}
		
		this.devices.flush();
		
		for (var i = 0; i < this.services.array.length; i++)
		{
			var service = this.services.array[i];
			service.updateJournals();
		}
	},

	start: function() //public
	{
		for (var id in this.apps)
		{
			var app = this.apps[id];
			app.start();
		}
		
		this.timer.callback = this.update.bind(this);
		this.timer.start();
	},

	//we can't run all init logic at construction as first the classes to be instantiated must be declared in the disjunction object's class packages.
	initialise: function(useMarkupPrefix)
	{
		if (this.WINDOW_CLASSES)
		{
			for (var c in Disjunction.Core) //recopy, this time include all inputs
			{
				window[c] = Disjunction.Core[c];
			}
			for (var e in Disjunction.Extensions) //recopy, this time include all inputs
			{
				window[e] = Disjunction.Extensions[e];
			}
		}
		if (this.WINDOW_CONSTANTS)
		{
			for (var c in this.constants) //recopy, this time include all inputs
			{
				window[c] = this.constants[c];
			}
		}
		this.builder 	= new Disjunction.Core.Builder(this.apps);
		this.builder.setTagNames(useMarkupPrefix);
		this.services 	= new Disjunction.Core.ServiceHub(this);
		this.devices 	= new Disjunction.Core.DeviceHub(this);
		
		
	},
	
	go: function(useMarkupPrefix)
	{
		this.initialise(useMarkupPrefix);
		this.builder.buildAll(document, this);
		this.start();
	},
	
	dispose: function()
	{
		this.services.dispose();
		this.devices.dispose();
	},
	
	addApp: function(id, app)
	{
		this.apps[id] = app;
	},
	
	removeApp: function(id)
	{
		this.apps[id].dispose();
		delete this.apps[id];
	}
};

//TODO send an options object on Build or constructor
Disjunction.Core.Builder = function(apps, options)
{
	options = options || {
		viewPrefix: '',
		viewSuffix: 'View',
		ctrlPrefix: '',
		ctrlSuffix: 'Ctrl',
		modelPrefix: '',
		modelSuffix: 'Model'
	};
	
	var prefix;
	var commonTagName;
	var appTagName;
	var modelTagName;
	var viewTagName;
	var ctrlTagName;
	var devicesTagName;
	var deviceTagName;
	var servicesTagName;
	var serviceTagName;
	var pointerTagName;
	var timerTagName;
		
	this.setTagNames = function(usePrefix)
	{
		prefix			= usePrefix ? 'dj-' : '';
		
		commonTagName 	= prefix + 'common';
		appTagName 		= prefix + 'app';
		modelTagName 	= prefix + 'model';
		viewTagName 	= prefix + 'view' + (prefix === '' ? '-' : ''); //sadly necessary due to the fact that custom element need hyphens.
		ctrlTagName		= prefix + 'ctrl';
		devicesTagName 	= prefix + 'devices';
		deviceTagName 	= prefix + 'device';
		servicesTagName = prefix + 'services';
		serviceTagName 	= prefix + 'service';
		pointerTagName 	= prefix + 'pointer';
		timerTagName 	= prefix + 'timer';
		
		document.registerElement(viewTagName, {
			prototype: Object.create(HTMLDivElement.prototype),
			extends: 'div'
		});
	}
	
	this.tabIndex = undefined;

	var app = undefined;
	var baseContext  = undefined; //the object on which an app's classes may be found
	var appConstantsContext = undefined; //the object one which to put constant device & service indexing properties
	//var djClassContext; //the object on which dj's classes may be found
	
	var viewIDs = {};
	var ctrlIDs = {}; //we need to treat Ctrl IDs separately as in C we will not have the global DOM search, but rather ID search within <view> root and <ctrl> root
	
	this.buildCommon = function(containerDOM, disjunction)
	{
		baseContext = window;
		
		var commonDOM = containerDOM.getElementsByTagName(commonTagName)[0];
		
		//Services
		var commonServicesDOM = commonDOM.getElementsByTagName(servicesTagName)[0];
		if (commonServicesDOM) //NOT always true
			this.addServices(commonServicesDOM, disjunction.services);
			
		//Devices
		var commonDevicesDOM = commonDOM.getElementsByTagName(devicesTagName)[0];
		if (commonDevicesDOM) //should always be true
			this.addDevices(commonDevicesDOM, disjunction.devices);
		else
			throw "Error: <"+devicesTagName+"> must be specified, containing at least one <"+deviceTagName+">.";
			
		//Pointer
		var domPointer = commonDOM.getElementsByTagName(pointerTagName)[0];
		if (domPointer)
		{
			if (domPointer.hasAttribute('device') &&
				domPointer.hasAttribute('x') &&
				domPointer.hasAttribute('y') &&
				domPointer.hasAttribute('select'))
			{
				var deviceIndexName = domPointer.getAttribute('device');
				var xChannelIndexName = domPointer.getAttribute('x');
				var yChannelIndexName = domPointer.getAttribute('y');
				var selectChannelIndexName = domPointer.getAttribute('select');
				
				var deviceIndex = disjunction.constants[deviceIndexName];
				var xChannelIndex = disjunction.constants[xChannelIndexName];
				var yChannelIndex = disjunction.constants[yChannelIndexName];
				var selectChannelIndex = disjunction.constants[selectChannelIndexName];
				
				//app.setPointer(window[device], window[x], window[y], window[select]);
				disjunction.setPointer(deviceIndex, xChannelIndex, yChannelIndex, selectChannelIndex);
			}
			else
				throw "Error: <pointer> must have attributes 'device', 'x', 'y', 'select'.";
		}
		else
			throw "Error: <pointer> must be specified, referencing a valid <device>.";
			
		//Timer
		var timerDOM = commonDOM.getElementsByTagName(timerTagName)[0];
		var classNamesJoined = timerDOM.className;
		var periodSec = parseFloat(timerDOM.getAttribute('period'));
		if (classNamesJoined.length > 0)
		{
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			
			var Class = this.getObjectFromPathString(baseContext, className+'Timer');
			if (Class)
			{
				disjunction.timer = new Class(periodSec);
			}
		}
	}
	
	this.buildAll = function(dom, disjunction)
	{
		
		this.buildCommon(dom, disjunction);
		this.buildApps(dom, disjunction);
		
		viewIDs = {};
		ctrlIDs = {};
	}
	
	this.buildApps = function(containerDOM, disjunction)
	{
		var appDOMs = containerDOM.getElementsByTagName(appTagName);

		var length = appDOMs.length;
		for (var i = 0; i < length; i++)
		{
			var appDOM = appDOMs[i];
			if (appDOM.id === "") //not a zero-length string
			{
				throw "Error: All <"+appTagName+"> elements must have an id attribute.";
			}
			
			var app = this.buildApp(appDOM, disjunction);
			apps[app.id] = app;
		}
		
		app = undefined; //clear member
	}
	
	//build. usage: <body onload="(new Builder()).buildApp(document.getElementById('Tagger'));">
	this.buildApp = function(appDOM, disjunction)
	{
		//app
		var id = appDOM.id;
		console.log(app);
		app = new Disjunction.Core.App(id, disjunction);
		console.log(app);
		var classNamesJoined = appDOM.className;
		console.log(classNamesJoined);
		var className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			className = classNames[0];
		}
		else
			throw "Error: App must have a class attribute set.";
		
		baseContext = window[className] || window; //TODO allow dot-separated package name?
		appConstantsContext = window;//baseContext; //for now, use same.

		//model
		var domModels = appDOM.getElementsByTagName(modelTagName);
		var domModel = domModels[0];
		if (domModels.length != 1)
			throw "Error: There can be only one <"+modelTagName+"> per <"+appTagName+">.";
		else
			app.model = this.addModel(domModel);
			
		//services - after model for injection, but before ctrl / view so may be accessed in constructor
		var domServices = appDOM.getElementsByTagName(servicesTagName)[0];
		if (domServices)
			this.addServices(domServices, app.services, app.model);

		//view
		var domViewRoots = appDOM.getElementsByTagName(viewTagName);
		var temp = [];
		//make sure only one exists at root depth!
		for (var i = 0 ; i < domViewRoots.length; i++)
		{
			var domView = domViewRoots[i];
			if (domView.parentNode.tagName.toLowerCase() === appTagName)
				temp.push(domView);
		}
		domViewRoots = temp;
		if (domViewRoots.length != 1)
			throw "Error: There can be only one root <"+viewTagName+"> per <"+appTagName+">. All <"+viewTagName+">s must be specified under this root.";
		else
			app.view = this.addView(domViewRoots[0], app.model);
		
		//ctrl
		var domCtrlRoots = appDOM.getElementsByTagName(ctrlTagName);
		var temp = [];
		//make sure only one exists at root depth!
		for (var i = 0 ; i < domCtrlRoots.length; i++)
		{
			var domCtrl = domCtrlRoots[i];
			if (domCtrl.parentNode.tagName.toLowerCase() === appTagName)
				temp.push(domCtrl);
		}
		domCtrlRoots = temp;
		if (domCtrlRoots.length != 1)
			throw "Error: There can be only one root <"+ctrlTagName+"> per <"+appTagName+">. All other <"+ctrlTagName+">s must be specified under this root.";
		else
			app.ctrl = this.addCtrl(domCtrlRoots[0], app.model);//, app.view);
		
		app.ctrl.app = app;
		
		//input bindings (per app)
		//TODO should these be specified on body vs. appDOM? they are shared between all apps.		
		for (var i = 0; i < disjunction.devices.array.length; i++)
		{
			var device = disjunction.devices.array[i];
			device.bindToDOM(appDOM); //null method if the given device does not work with DOM
		}
		
		//store / return app
		apps[app.id] = app;
		return app;
	}
	
	//(JS-only) Multi-app pages
	this.buildByIds = function(ids)
	{	
		for (var i in ids)
		{
			var id = ids[i];

			var app = this.buildById(id);
		}
	}
	
	//find and build
	this.buildById = function(id)
	{
		var appDOM = document.getElementById(id);
		return this.buildApp(appDOM, disjunction);
	}
	
	this.addDevices = function(containerDOM, devices)
	{
		for (var i = 0; i < containerDOM.children.length; i++)
		{
			var dom = containerDOM.children[i];
			if (dom.tagName.toLowerCase() === deviceTagName)
			{
				var className = dom.className;
				var Class = /*Disjunction.Extensions[className];*/ this.getObjectFromPathString(baseContext, className/*+'Device'*/);
				if (Class)
				{
					var deviceConstantName = 'DEVICE_'+className.replace('Device','').toUpperCase();
					var device = devices.add(new Class());
					disjunction.constants[deviceConstantName] = device;
					
					//TODO remove and adapt to allow the context for placement to be specified
					if (disjunction.WINDOW_CONSTANTS)
						window[deviceConstantName] = device;
				}
			}
		}
	}
	
	this.addServices = function(domContainer, services, model)
	{
		for (var i = 0; i < domContainer.children.length; i++)
		{
			var dom = domContainer.children[i];
			this.addService(dom, services, model);
		}
	}
	
	this.addService = function(dom, services, model)//, parentDomRect)
	{
		//get model
		if (dom.hasAttribute('model'))
		{
			var pathJoined = dom.getAttribute('model');
			model = this.getObjectFromPathString(model, pathJoined);
			console.log(service, model);
		}
		
		var service;
		var className;
		
		if (dom.tagName.toLowerCase() === serviceTagName)
		{
			var shortServiceName = dom.className;
			var longServiceName = dom.className + 'Service';
			className = longServiceName;
			var Class = this.getObjectFromPathString(baseContext, longServiceName); //TODO make the object on which to put this, optional by parameter
			if (Class)
			{
				//construct service
				var serviceConstantName = 'SERVICE_'+shortServiceName.toUpperCase();
				service = new Class(app, model);
				service.className = shortServiceName;
				serviceIndex = services.add(service);
				
				console.log(service);
				
				//assign by constant identifier
				appConstantsContext[serviceConstantName] = serviceIndex;
			}
		}
		
		if (!service)
			throw "Error: Cannot find class '"+className+"'.";
	}
	
	this.addModel = function(dom)
	{
		var model;
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = options.modelPrefix + classNames[0] + options.modelSuffix;
			var Class = this.getObjectFromPathString(baseContext, className);
			if (Class)
			{
				model = new Class();
			}
		}
		
		return model;
	}
	
	this.addCtrl = function(dom, model)//, view)
	{
		//get model
		if (dom.hasAttribute('model'))
		{
			var pathJoined = dom.getAttribute('model');
			model = this.getObjectFromPathString(model, pathJoined);
		}
						
		var ctrl;
		
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = options.ctrlPrefix + classNames[0] + options.ctrlSuffix;
			var classNameShort = classNames[0];
			var firstChar = classNames[0][0]; //first char of first classname
			
			if (isNaN(firstChar)) //if first character is a letter...
			{
				if (firstChar === firstChar.toUpperCase()) //and it's uppercase...
				{
					//...then it's a classname
					var Class = this.getObjectFromPathString(baseContext, className);
					if (Class)
					{
						//construct ctrl
						ctrl = new Class(app, model);
						dom.ctrl = ctrl; //useful for debugging, mirrors View approach
						ctrl.className = classNameShort;
						ctrl.model = model; //by default, set to what was passed in; elaborate (below) if necessary
						
						//recurse children
						for (var a = 0; a < dom.children.length; a++)
						{
							var childDOM = dom.children[a];
							var childCtrl = this.addCtrl(childDOM, ctrl.model);
							if (childCtrl)
								ctrl.addChild(childCtrl);
							
							//TODO inheritance of view/model from parent ctrl
						}
					}
				}
			}
		}
		
		if (!ctrl)
			throw "Error: Cannot find class '"+className+"'.";
		
		return ctrl;
	}
	
	/**
	 * Creates a View hierarchy. If certain elements do not have a class with which to instantiate a View, they will be skipped;
	 * however their descendants will still be processed and will set the last ancestral View as their parent.
	 * @return The {Disjunction.Core.View} if one was added; otherwise undefined.
	 */
	this.addView = function(dom, model)
	{
		//get model
		if (dom.hasAttribute('model'))
		{
			var pathJoined = dom.getAttribute('model');
			model = this.getObjectFromPathString(model, pathJoined);
		}
		
		var view;
	
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = options.viewPrefix + classNames[0] + options.viewSuffix;
			var firstChar = classNames[0][0]; //first char of first classname
			
			if (isNaN(firstChar)) //if first character is a letter...
			{
				if (firstChar === firstChar.toUpperCase()) //and it's uppercase...
				{
					//...then it's a classname
					var Class = this.getObjectFromPathString(baseContext, className); //TODO make object where this is found, optional.
					if (Class)
					{
						view = new Class(app, model);

						//any DOM element with a legitimate View attached, needs this
						dom.onfocus = function()
						{
							this.view.focus();
						}
						//dom.onblur = function()
						//{this.view.wasFocused = this.view.isFocused;
						//this.view.isFocused = false;};
					}
					else
						throw "Error: Cannot find class '"+className+"'.";
				}
			}
		}
		
		if (!view)
		{
			view = new Disjunction.Core.NullView(app, model); //null / flyweight
		}

		view.dom = dom;
		dom.view = view;
		
		//recurse children
		for (var a = 0; a < dom.children.length; a++)
		{
			var childDOM = dom.children[a];
			var childView = this.addView(childDOM, view.model);
			view.addChild(childView);
			
			//TODO inheritance from parent view
		}

		return view;
	}
	
	this.getObjectFromPathString = function(object, pathJoined)
	{
		var path = pathJoined.split('.');
		if (path[0] === "") path.shift();
		if (path[path.length-1] === "") path.pop();
		var pathIncremental = ""; //for error throw only
		
		//walk down the object tree to the appropriate property
		while (path.length > 0)
		{
			var property = path.shift();
			if (object.hasOwnProperty(property))
			{
				object = object[property];
				pathIncremental += '.' + property;
			}
			else
				throw "Error: "+object+pathIncremental+" does not have property '"+property+"'.";
		}
		return object;
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Builder = Disjunction.Core.Builder;
	
/**
 *	@const @function @constructs Disjunction.Core.Journal
 *	@param {Array} states The states array.
 *	@param {Array} deltas The deltas array.
 */
Disjunction.Core.Journal = function(states, deltas)
{
	this.states = states;
	this.deltas = deltas;
	
	this.stateLocked = false;
	this.deltaLocked = false;
};

/**
 *	Unlock the Journal.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.unlock = function() //framework use only!
{
	this.stateLocked = this.deltaLocked = false;
}

//TODO use accessors once they get a boost in performance http://jsperf.com/defineproperties-vs-prototype, https://bugzilla.mozilla.org/show_bug.cgi?id=772334

/**
 *	Get the current state.
 *  @const @function
 */
Disjunction.Core.Journal.prototype.state = function()
{
	var states = this.states;
	return states[0];
}

/**
 *	Get the current delta.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.delta = function()
{
	var deltas = this.deltas;
	return deltas[0];
}

/**
 *	Get the last state.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.stateLast = function()
{
	var states = this.states;
	return states[1];
}

/**
 *	Get the last delta.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.deltaLast = function()
{
	var deltas = this.deltas;
	return deltas[1];
}

/**
 *	Get the state at the specified index.
 *	@const @function
 *	@param {Number} index The index into the states array.
 *	@returns {*} The specified state.
 */
Disjunction.Core.Journal.prototype.stateAt = function(index)
{
	var states = this.states;
	return states[index];
}

/**
 *	Get the delta at the specified index.
 *	@const @function
 *	@param {Number} index The index into the deltas array.
 *	@returns {*} The specified delta.
 */
Disjunction.Core.Journal.prototype.deltaAt = function(index)
{
	var deltas = this.deltas;
	return deltas[index];
}

/**
 *	Set a state.
 *	@const @function
 *	@param {*} value The value to set as current.
 *	@param {Number} [index=0] The index to set to the specified value.
 */
Disjunction.Core.Journal.prototype.setState = function(value, index)
{
	console.log('setState', value);
	/*
	if (this.stateLocked)
	{
		throw "Error: Journals may only be updated once per App.update().";
	}
	else
		this.stateLocked = true;
	*/
	index = index || 0;
	var states = this.states;
	states[index] = value;
}

/**
 *	Set a delta.
 *	@const @function
 *	@param {*} value The value to set as current.
 *	@param {Number} [index=0] The index to set to the specified value.
 */
Disjunction.Core.Journal.prototype.setDelta = function(value, index)
{
	if (this.deltaLocked)
		throw "Error: Journals set to lockOnUpdate may only be updated once before locks clear (on App.update()).";
	else
		this.deltaLocked = true;

	index = index || 0;
	var deltas = this.deltas;
	deltas[index] = value;
}

/**
 *	Set all the Journal's states.
 *	@const @function
 *	@param {Number} [index=0] value The value to set all elements to.
 */
Disjunction.Core.Journal.prototype.setStates = function(value)
{
	var states = this.states;
	for (var i = 0; i < states.length; i++)
		states[i] = value;
}

/**
 *	Set all the Journal's deltas.
 *	@const @function
 *	@param {*} value The value to set all elements to.
 */
Disjunction.Core.Journal.prototype.setDeltas = function(value)
{
	var deltas = this.deltas;
	for (var i = 0; i < deltas.length; i++)
		deltas[i] = value;
}

/**
 *	Check whether the Journal has changed between current value and most recent historical value.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.justChangedState = function()
{
	return this.changedState(0, 1);
}


//VIRTUAL / OVERRIDABLE
//TODO have default implementations in separate files
/**
 *	Update the Journal's non-current values, and calculate new delta from newest states, or new state from last state + current delta.
 *	@abstract @function
 */
Disjunction.Core.Journal.prototype.shift = function() {}

/**
 *	Check whether the Journal has changed between two designated indices.  Override to change the way equality checking is performed.
 *	@abstract @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Core.Journal.prototype.changedState = function(from, to){}


if (disjunction.WINDOW_CLASSES) 
	window.Journal = Disjunction.Core.Journal;

//SERVICES
Disjunction.Core.Service = function(app, model)
{
	this.app = app;
	this.model = model;
	
	this.models = []; /** Models belonging exclusively to this Service, to allow for their Journals to be updated. DO NOT add the injected model to this or it will be updated twice! */
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}

if (disjunction.WINDOW_CLASSES) 
	window.Service = Disjunction.Core.Service;
	
Disjunction.Core.ServiceHub = function()
{
	this.array = [];
	
	this.get = function(i)
	{
		return this.array[i];
	}
	
	/**
	 *	Add the Service to this hub.
	 *	@return The newly-allocated index of the Service.
	 */
	this.add = function(service)
	{
		var array = this.array;
		array.push(service);
		return array.length - 1;
	}
	
	this.dispose = function()
	{
		var array = this.array;
		for (var i = 0; i < array.length; i++)
		{
			array[i].dispose();
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.ServiceHub = Disjunction.Core.ServiceHub;
	
//DEVICES
//TODO listen for connecting / disconnecting devices
Disjunction.Core.DeviceHub = function(dom)
{
	//TODO actually in C we should simply realloc when devices are added, and in JS just use array length after pushes.
	//use sufficiently large number for devices
	this.array = [];
	
	this.get = function(i)
	{
		return this.array[i];
	}
	
	this.poll = function()
	{
		var numDevices = this.array.length;
		for (var i = 0; i < numDevices; i++)
		{
			var device = this.array[i];
			device.poll();
		}
	}

	this.flush = function()
	{
		var numDevices = this.array.length;
		for (var i = 0; i < numDevices; i++)
		{
			var device = this.array[i];
			if (device.eventBased)
			{
				var numChannels = device.channels.length;
				for (var j = 0; j < numChannels; j++)
				{
					var channel = device.channels[j];
					channel.delta = 0;
				}
			}
		}
	}
	
	/**
	 *	Add the Device to this hub.
	 *	@return The newly-allocated index of the Device.
	 */
	this.add = function(device)
	{
		var array = this.array;
		array.push(device);
		return array.length - 1;
	}
	
	this.dispose = function()
	{
		var array = this.array;
		for (var i = 0; i < array.length; i++)
		{
			array[i].dispose();
		}
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.DeviceHub = Disjunction.Core.DeviceHub;
	
Disjunction.Core.DeviceChannel = function()
{
	this.value = 0;
	this.delta = undefined;
	//JS only...
	this.blocked = false;
	this.block = function() {this.blocked = true;};
	this.unblock = function() {this.blocked = false;};
};
if (disjunction.WINDOW_CLASSES) 
	window.DeviceChannel = Disjunction.Core.DeviceChannel;
	
Disjunction.Core.Device = function(numChannels) //ABSTRACT / INTERFACE
{
	this.channels = [];
	this.numChannels = numChannels;
	this.eventBased = true; //override; in JS, true by default.
	
	//constructor
	for (var i = 0; i < this.numChannels; i++)
	{
		this.channels[i] = new DeviceChannel();
	}
	
	//passive: use as an input event handler - DOM
	this.receive = function(event)
	{
		//ABSTRACT
	}

	//active: poll underlying system on global update
	this.poll = function()
	{
		//ABSTRACT
	}
	
	this.bindToDOM = function(dom)
	{
		//ABSTRACT
	}
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}
if (disjunction.WINDOW_CLASSES) 
	window.Device = Disjunction.Core.Device;
	
//IMPLEMENTATION-SPECIFIC

Disjunction.Core.App = function(id, disjunction) //final
{
	this.id = id; //for compound apps
	this.disjunction = disjunction;
	
	this.services = new Disjunction.Core.ServiceHub(this);
	this.model = undefined;
	this.view = undefined;
	this.ctrl = undefined;
	
	this.focus = undefined; //focus may be reached in many ways -- not just pointer. it's just the active view used for input.
	
	
	this.start = function() //final
	{
		this.ctrl.start();
		this.view.startRecurse();
		this.focus = this.view;
		
		this.focus.dom.focus(); //triggers View.focus() which changes pointer focus (see Builder) -- necessary as in DOM JS we cannot control focus events due to mouse or tabbing.
		//this.view.focus(); //the expected version... outside DOM JS this will work.
	}
	
	this.update = function() //final
	{	
		//console.log('------A');
		var model = this.model;
		var view = this.view;
		var pointer = disjunction.pointer;
		if (pointer) 
		{
			if (view.enabled) //root enabled
			{
				pointer.findTarget(view);
				pointer.updateSelected();
			}
		}
		
		//check whether focus has changed
		//this may occur externally from DOM onfocus event handler, or be done internally based on pointer / input events. 
		//if (this.pollFocus)
		//	this.pollFocus();
		
		//TODO set focus through *user-defined* approaches 
		if (pointer.target && pointer.isSelected)
			pointer.target.focus();
			
		var focus = this.focus;
		
		//root ctrl operates on root model and focused view
		var ctrl = this.ctrl;
		//note we do not pass Pointer in -- we need the focused View for its input, which exists irrespective of whether there is a Pointer available or not
		/*var input = */focus.input(this, model); //for cases where input is not DOM- or Pointer-based.
		ctrl.update();//TODO should only accept the input along with view name - not the whole View that produced the input. Ctrl should not have access to View.

		view.outputRecurse(this, model); //root view, recurse
		
		ctrl.refresh();
		//console.log('------B');
	}
	
	//TODO figure out how to handle distinction between stop and dispose, at every level (dj object downward)
	this.dispose = function() //final
	{
		ctrl.stop(); //TODO if not already stopped
		services.dispose();
		view.dispose();
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.App = Disjunction.Core.App;
	
Disjunction.Core.Model = function()
{
	this.journals = []; //array of arrays TODO array of Journal (inheriting from Array for faster internal access) to support offsets into an existing, large (typed) array

	this.progressJournals = function()
	{
		var journals = this.journals;
		var length = journals.length;
		for (var i = 0; i < length; i++)
		{
			var journal = journals[i];
			
			//journal.shiftStates();
			//journal.shiftDeltas();
			journal.shift(); //allows combining of state and delta shift loops for fewer conditionals
			//journal.clear();
			journal.unlock();
		}
	}
	
	//Abstract methods
	
	this.serialise = this.serialize = function()
	{
	}
	
	this.deserialise = this.deserialize = function(serialised)
	{
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Model = Disjunction.Core.Model;
	
Disjunction.Core.View = function(app, model)
{
	this.app = app;
	this.model = model;

	this.enabled = true; //used by timing mechanism to know whether to update or not
	this.parent = undefined;
	this.children = []; //Does NOT represent draw order unless user renderer specifically does so.
	//this.childrenByName = {}; //should be private
	this.childrenByClassName = {}; //should be private
	this.root = undefined; //TODO remove?
	this.body = undefined; //pixel or 3D geometry, typically Box2 or 3 - explicitly injected after view construction... could be bitmap or arbitrary tri mesh
	this.bounds = undefined; //always necessary for first pass pointer detection, and pointer transformations -- may be defined by body

	//ABSTRACT METHODS
	/** Used to set up resources or values specific to this View. */ 
	this.start = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	/** Used to clean up resources or reset values for this View, if it is no longer needed and can be released. */ 
	this.stop = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	/** (When focused) Process all input into this View and modify model and View state (if using a persistent / retained mode display list) accordingly, if View enabled. **/
	this.input = function()//app, model)
	{
		//ABSTRACT: OVERRIDE
		//-run only for the currently focused view in a given global update
	}

	/** Render all output for this View based on model state, on View tree walk-down, if View enabled. **/
	this.output = function()//app, model)
	{
		//ABSTRACT: OVERRIDE
	}

	/** Render all output for this View based on model state, on View tree walk-up, if View enabled. **/
	/** Occurs after recursive, collective update of subviews. Use this if you used update(pre) to gather information onto a single canvas, and want to render that canvas in retrospect. **/
	this.outputPost = function(app, model)
	{
		//ABSTRACT: OVERRIDE
	}
	
	//restores visibility.
	this.show = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	//keeps view processing even if its not visible.
	this.hide = function()
	{
		/* ABSTRACT: OVERRIDE */
	}
	
	this.dispose = function()
	{
		//ABSTRACT
	}
	
	//FINAL METHODS
	//re-enables view processing.
	this.enable = function()
	{
		this.enabled = true;
		this.show();
	}
	
	//stops view processing altogether.
	this.disable = function()
	{
		this.enabled = false;
		this.hide();
	}
	
	this.outputRecurse = function(deltaSec) //final
	{
		this.output(deltaSec);
	
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				if (child.enabled)
					child.outputRecurse(deltaSec);
			}
		}
		
		this.outputPost(deltaSec);
	}
	
	//TODO Remove this. Remember that DOM render order works on zIndex. The below is an immediate-mode rendering solution. We cannot assume either in generalised framework code.
	//NB http://philipwalton.com/articles/what-no-one-told-you-about-z-index/ ... there is no simple way to manage global DOM depths, and it's safer to leave this within the domain of user code.
	/*
	this.bringToFore = function(child)
	{
		var index = this.children.indexOf(child);
		if (index > -1)
		{
			var temp = children[i];
			children[i] = children[j];
			children[j] = temp;
		}
	}
	*/
	
	this.isRoot = function() //final
	{
		return this == this.root;
	}
	
	this.addChild = function(childView) //final
	{
		this.children.push(childView);
		//this.childrenById[childView.id] = childView;
		var classNamesJoined = childView.dom.className;
		if (classNamesJoined.length > 0)
		{
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			
			//create array in map, if it doesn't already exist.
			if (!this.childrenByClassName.hasOwnProperty(className))
				this.childrenByClassName[className] = [];
				
			var array = this.childrenByClassName[className];
			
			array.push(childView);
		}
		
		childView.parent = this;
		childView.root = this.root;
		childView.app = this.app;
		//childView.model = this.model;
	}
	
	/*
	//since Views are typically only added at compile/build time, and may be enabled / disabled, we don't need to remove any.
	this.removeChild = function(childView) //final
	{
		this.children.push(childView);
		this.childrenByName[childView.id] = undefined;
		childView.parent = undefined;
		childView.root = undefined;
		childView.app = undefined;
		childView.model = undefined;
	}
	*/
	
	//C-compatible array search to find child by id.
	//TODO use a hashmap? ...external access remains exactly the same.
	this.getChildById = function(id)
	{
		//return this.childrenByName[id];
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			if (child.id === id)
			{
				return child;
			}
		}
		
		return undefined;
	}
	
	this.getChildrenByClassName = function(className)
	{
		return this.childrenByClassName[className];
	}
	
	this.hasChildren = function()
	{
		return this.children.length > 0;
	}
	
	this.startRecurse = function() //final
	{
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				child.startRecurse();
			}
		}
		
		this.start(); //create from bottom up
	}
	
	this.stopRecurse = function(deltaSec) //final
	{
		this.stop();
	
		var children = this.children;
		if (children)
		{
			var length = children.length;
			for (var i = 0; i < length; i++)
			{
				var child = children[i];
				child.stopRecurse();
			}
		}
	}

	
	this.getDepth = function()
	{
		var depth = 0;

		var ancestor = this.parent;
		while (ancestor)
		{
			depth++;
			ancestor = ancestor.parent;
		}
		
		return depth;
	}
	
	this.getLeaves = function()
	{
		//DFS
		var leaves = [];
		if (this.children.length == 0)
			leaves.push(this);
		else
		{
			for (var i = 0; i < this.children.length; i++)
			{
				var child = this.children[i];
				var childLeaves = child.getLeaves();
				leaves.extend(childLeaves); //add into array to be returned
			}
		}
		return leaves;
	}
	
	this.getLeavesEnabled = function()
	{
		//DFS
		var leaves = [];
		
		if (this.enabled)
		{
			if (this.children.length == 0)
				leaves.push(this);
			else
			{
				for (var i = 0; i < this.children.length; i++)
				{
					var child = this.children[i];
					var childLeaves = child.getLeaves();
					leaves.extend(childLeaves); //add into array to be returned
				}
			}
		}
		return leaves;
	}
	
	this.isLeaf = function()
	{
		return this.children == undefined;
	}
	
	//--- transformation functions -- later to go in Transform abstract class (2D or 3D) ---//
	
	//remember to clone before running values through these functions
	//position is in local coordinates for this object.
	this.toChild = function(position, child)
	{
		position.x -= child.bounds.x0;
		position.y -= child.bounds.y0;
	}
	
	//position is in local coordinates for child object.
	this.fromChild = function(position, child)
	{
		position.x += child.bounds.x0;
		position.y += child.bounds.y0;
	}
	
	//position is in local coordinates for this object.
	this.toParent = function(position)
	{
		position.x += this.bounds.x0;
		position.y += this.bounds.y0;
	}

	//position is in local coordinates for parent object.
	this.fromParent = function(position)
	{
		position.x -= this.bounds.x0;
		position.y -= this.bounds.y0;
	}
	
	//position is in world coordinates.
	this.fromWorld = function(position)
	{	
		var path = [];
		var view = this; //could be root, otherwise...
		path.push(view);
		while (view.parent)
		{
			view = view.parent; //...move up to root.
			path.unshift(view); //root down order
		}
		
		//walk down, transforming
		for (var i = 0; i < path.length; i++)
		{
			var view = path[i];
			position.x -= view.bounds.x0;
			position.y -= view.bounds.y0;
			//console.log('++', i, position, view.bounds.x0);
		}
	}
	
	//position is in local coordinates for this.
	this.toWorld = function(position, parent)
	{
		var view = this; //could be root, otherwise...
		while (view.parent)
		{
			view = view.parent; //...move up to root.
			position.x += view.bounds.x0;
			position.y += view.bounds.y0;
		}
	}

	this.focus = function()
	{
		this.app.focus = this;
	}
	
	//JS - should be in extensions?
	
	//from http://bradsknutson.com/blog/javascript-detect-mouse-leaving-browser-window/
	this.addDOMEvent = function(obj, evt, fn)
	{
		if (obj.addEventListener) {
			obj.addEventListener(evt, fn, false);
		}
		else if (obj.attachEvent) {
			obj.attachEvent("on" + evt, fn);
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.View = Disjunction.Core.View;

Disjunction.Core.Ctrl = function(app, model)
{
	this.app = app;
	this.model = model;
	
	this.parent = undefined;
	this.children = []; //processing order, if not accessed individually by ids

	/** Tells whether this Task is due to start running; typically a check on model, input devices, Pointer. 
	 * 	@return {Boolean}
	 */ 
	this.isToStart = function()
	{
		//abstract
		//check model and input to see whether to start
	}
	
	/** Tells whether this Task is due to stop running; typically a check on model, input devices, Pointer.
	 * 	@return {Boolean}
	 */
	this.isToStop = function()
	{
		//abstract
		//check model and input to see whether to stop
	}
	
	/** Start this Timeline -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.start = function()
	{
		//abstract
	}
	
	/** Stop this Timeline -- user code to perform one off tasks such as Model member destruction or reset, or View state change. */ 
	this.stop = function()
	{
		//abstract
	}
	
	/** Update Model based on other Model values, on every frame from start()ed till stop()ed (including during the former, but excluding during the latter). */ 
	this.update = function(focus)
	{
		//abstract
	}
	
	/** Cleanse the model of any ephemeral values (usually deltas affecting View outputs), before the next global update. */ 
	this.refresh = function(focus)
	{
		//abstract
	}
	
	//*** FINAL METHODS ***//
	
	this.addChild = function(child) //final
	{
		this.children.push(child);
		child.parent = this;
		//this.childrenById[child.id] = child;
	}
	
	//There will never be multiple Ctrls of same type (as child of a single parent), so we need not use ID.
	this.getChildByClassName = function(className) //final
	{
		//return this.childrenByName[id];
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			if (child.className === className)
			{
				return child;
			}
		}
		
		return undefined;
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Ctrl = Disjunction.Core.Ctrl;
	
Disjunction.Core.Timer = function()
{	
	this.lastUpdateSec = undefined;
	this.callback = undefined;
	this.deltaSec = undefined;
	
	this.start = function()
	{
		//ABSTRACT
	}
	
	this.stop = function()
	{
		//ABSTRACT
	}
	
	this.update = function()
	{
		//calc delta time
		var thisUpdateSec = new Date().getTime();
		var lastUpdateSec = this.lastUpdateSec;
		this.lastUpdateSec = thisUpdateSec;
        var deltaSec = this.deltaSec = (thisUpdateSec - (lastUpdateSec || thisUpdateSec)) / 1000;
		
		if (this.callback)
		{
			this.callback(deltaSec); //TODO remove param, no longer used
			
			//TODO should simply be: disjunction.update(); -- we never use timers for anything else
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Timer = Disjunction.Core.Timer;
	
//Global Pointer data:
//holds information about the pointer device
//knows which ONE element is currently pointed to, out of the entire tree (target, not focus)
//checks if that target has changed
//abstracts concepts of pointer to be device-agnostic

Disjunction.Core.Pointer = function() //ABSTRACT / INTERFACE
{
	this.device = undefined;
	this.isSelected = false;
	this.wasSelected = false;
	
	this.target = undefined;
	this.targetLast = undefined;
	this.targetSelected = undefined;
	this.targetReleased = undefined;
	this.positionInTarget;// = new Point2(); //local position of pointer within pointed element //TODO rename to positionLocal
	this.position = new Disjunction.Core.Point2(); //world position of pointer within pointed element //TODO rename to positionWorld
	this.entered = undefined;
	this.exited = undefined;
	
	this.xChannel = undefined;
	this.yChannel = undefined;
	this.selectChannel = undefined;
	
	this.dragging = undefined;
	
	this.targetSupplied = undefined;
	
	this.startDragTarget = function()
	{
		this.dragging = this.target;
	}
	
	this.stopDragTarget = function()
	{
		this.dragging = undefined;
	}
	
	this.setTargetLast = function()
	{
		this.targetLast = this.target;
	}
	
	this.progressDrag = function()
	{
		this.dragging.bounds.x0 += this.xChannel.delta;
		this.dragging.bounds.x1 += this.xChannel.delta;
		this.dragging.bounds.y0 += this.yChannel.delta;
		this.dragging.bounds.y1 += this.yChannel.delta;
	}
	//generally called by view to do some animation such as border highlight
	this.hasChangedTarget = function()
	{
		return this.target !== this.targetLast;
	}
	
	this.hasChangedSelected = function()
	{
		return this.isSelected != this.wasSelected;
	}
	
	this.hasMoved = function()
	{
		return this.xChannel.delta != 0 || this.yChannel.delta != 0;
	}
	
	//see whether target we released over is same we selected over
	this.isReleasedSelectedTarget = function()
	{
		return targetSelected === targetReleased;
	}
	
	this.updateSelected = function() //TODO change for touch devices.
	{
		this.wasSelected = this.isSelected;
		if (this.selectChannel.delta > 0)
			this.isSelected = true;
		else if (this.selectChannel.delta < 0)
			this.isSelected = false;
		//...otherwise don't change select state.
	}
	
	this.findTarget = function(view)
	{
		//transform from parent into child coordinates
		//after all transformations down the hierarchy, the point is in the targeted View's coordinate system.
		//TODO later: scaling; custom transform centre
		var device = this.device;
		this.position.x = this.xChannel.value;
		this.position.y = this.yChannel.value;
//debugger;
		//TODO implement for 3 dimensions as well as 2.
		if (this.position.x !== undefined && this.position.y !== undefined) //often undefined for touch interfaces
		{
			if (this.intersectsView(view))
				this.target = view;
			
			if (this.target)
			{
				while (this.target.children.length > 0)
				{
					var children = this.target.children;

					var targetCandidates = [];
					for (var i = 0; i < children.length; i++)
					{
						var child = children[i];
						if (this.intersectsView(child))
							targetCandidates.push(child);
					}
					
					//break ties by selecting the highest index (last rendered)
					if (targetCandidates.length > 0)
					{
						this.target = targetCandidates[targetCandidates.length - 1];
					}
					else
						break;
				}
			}
			
			this.setTargetLast();
			
			if (this.target)
			{
				this.positionInTarget = this.position.clone();
				this.target.fromWorld(this.positionInTarget);
			}
		}
	}
	
	this.intersectsView = function(view)
	{
		var pointerPositionInTarget = this.positionInTarget = this.position.clone();
		//view.fromWorld(pointerPositionInTarget); //necessary even for root view, when it isn't a standard fullscreen, origin-positioned element.
		var inBounds = view.bounds.containsWorld(pointerPositionInTarget);
		var inGeometry = view.geometry ? view.geometry.intersects(pointerPositionInTarget) : true;
		return inBounds && inGeometry;
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Pointer = Disjunction.Core.Pointer;
	
Disjunction.Core.Point2 = function(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
	
	this.clone = function()
	{
		return new Disjunction.Core.Point2(this.x, this.y);
		 clone;
	}
};

Disjunction.Core.Point2.prototype.constructor = Disjunction.Core.Point2; //TODO remove?

if (disjunction.WINDOW_CLASSES) 
	window.Point2 = Disjunction.Core.Point2;
	
Disjunction.Core.NullView = function(app, model)
{
	Disjunction.Core.View.call(this, app, model); //extend base framework class
	
	this.bounds = new Box2();
	
	this.start = function() 
	{
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		this.onWindowResize();
	}
	
	this.onWindowResize = function()
	{
		var dom = this.dom;
		var boundingClientRect = dom.getBoundingClientRect();
		this.bounds.x0 = boundingClientRect.left;
		this.bounds.y0 = boundingClientRect.top;
		this.bounds.x1 = boundingClientRect.right;
		this.bounds.y1 = boundingClientRect.bottom;
	}
};

Disjunction.Core.NullView.prototype = Object.create(Disjunction.Core.View.prototype);
Disjunction.Core.NullView.prototype.constructor = Disjunction.Core.NullView;

if (disjunction.WINDOW_CLASSES) 
	window.NullView = Disjunction.Core.NullView;
