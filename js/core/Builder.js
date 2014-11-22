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

	var app = '123';
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
				service = new Class(model);
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