//TODO rename all dom/DOM to element/Element
Disjunction.Core.Builder = function(apps)
{
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

	var app;
	
	var viewIDs = {};
	var ctrlIDs = {}; //we need to treat Ctrl IDs separately as in C we will not have the global DOM search, but rather ID search within <view> root and <ctrl> root
	
	this.buildCommon = function(containerDOM, disjunction)
	{
		var commonDOM = containerDOM.getElementsByTagName(commonTagName)[0];
		
		var commonServicesDOM = commonDOM.getElementsByTagName(servicesTagName)[0];
		if (commonServicesDOM) //NOT always true
			this.addServices(commonServicesDOM, disjunction.services);
			
		var commonDevicesDOM = commonDOM.getElementsByTagName(devicesTagName)[0];
		if (commonDevicesDOM) //should always be true
			this.addDevices(commonDevicesDOM, disjunction.devices);
		else
			throw "Error: <"+devicesTagName+"> must be specified, containing at least one <"+deviceTagName+">.";
			
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
			
		var timerDOM = commonDOM.getElementsByTagName(timerTagName)[0];
		var classNamesJoined = timerDOM.className;
		var periodSec = parseFloat(timerDOM.getAttribute('period'));
		if (classNamesJoined.length > 0)
		{
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			
			var TimerClass = Disjunction.Extensions[className+'Timer']; //TODO fix Timer not to be in extensions, or fix className to allow a dot-separated name (less likely)
			if (TimerClass)
			{
				disjunction.timer = new TimerClass(periodSec);
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
		app = new Disjunction.Core.App(id, disjunction);

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
			app.ctrl = this.addCtrl(domCtrlRoots[0], app.model, app.view);
		
		app.ctrl.app = app;
		
		//input: special case devices for DOM JS
		var mouse = disjunction.devices.array[disjunction.constants.DEVICE_MOUSE];
		var keyboard = disjunction.devices.array[disjunction.constants.DEVICE_KEYBOARD];
		//TODO should these be specified on body vs. appDOM? they are shared between all apps.
		appDOM.addEventListener('mousedown', 	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mouseup',		ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mousemove',	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mousewheel',	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('keydown', 		ES5.bind(keyboard, keyboard.receive));
		appDOM.addEventListener('keyup',		ES5.bind(keyboard, keyboard.receive));
		
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
	
	this.addDevices = function(containerDOM, devices)//, parentDomRect)
	{
		for (var i = 0; i < containerDOM.children.length; i++)
		{
			var dom = containerDOM.children[i];
			if (dom.tagName.toLowerCase() === deviceTagName)
			{
				var className = dom.className;
				var Class = Disjunction.Extensions[className];
				if (Class)
				{
					var deviceConstantName = 'DEVICE_'+className.replace('Device','').toUpperCase();
					var device = devices.add(new Class());
					
					disjunction.constants[deviceConstantName] = device;
					if (disjunction.WINDOW_CONSTANTS)
						window[deviceConstantName] = device; //TODO make the object on which to put this, optional by parameter
				}
			}
		}
	}
	
	this.addServices = function(containerDOM, services, model)//, parentDomRect)
	{
		for (var i = 0; i < containerDOM.children.length; i++)
		{
			var dom = containerDOM.children[i];
			if (dom.tagName.toLowerCase() === serviceTagName)
			{
				var shortServiceName = dom.className;
				var longServiceName = dom.className + 'Service';
				var Class = window[longServiceName]; //TODO make the object on which to put this, optional by parameter
				if (Class)
				{
					var serviceConstantName = 'SERVICE_'+shortServiceName.toUpperCase();
					var service = services.add(new Class());
					
					//set model
					if (dom.hasAttribute('model'))
					{
						var pathJoined = dom.getAttribute('model');
						service.model = this.getModelFromPathString(model, pathJoined);
					}
					
					disjunction.constants[serviceConstantName] = service;
					if (disjunction.WINDOW_CONSTANTS)
						window[serviceConstantName] = service; //TODO make the object on which to put this, optional by parameter
				}	
			}
		}
	}
	
	this.addModel = function(dom)
	{
		var model;
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			className += 'Model';
			var Class = window[className];
			if (Class)
			{
				model = new Class();
			}
		}
		
		return model;
	}
	
	this.addCtrl = function(dom, model, view)
	{
		//console.log('model', model);
		var ctrl;
		
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			var classNameShort = className;
			var firstChar = className[0];
			
			if (isNaN(firstChar)) //if first character is a letter...
			{
				if (firstChar === firstChar.toUpperCase()) //and it's uppercase...
				{
					//...it's a classname
					className += 'Ctrl';
					var Class = window[className]; //TODO make object where this is found, optional.
					if (Class)
					{
						ctrl = new Class(app);
						dom.ctrl = ctrl; //useful for debugging, mirrors View approach
						ctrl.className = classNameShort;
						ctrl.model = model; //by default, set to what was passed in; elaborate (below) if necessary
						
						//set model
						if (dom.hasAttribute('model'))
						{
							var pathJoined = dom.getAttribute('model');
							ctrl.model = this.getModelFromPathString(ctrl.model, pathJoined);
						}
						
						//set view
						if (dom.hasAttribute('view'))
						{
							var pathJoined = dom.getAttribute('view');
							ctrl.view = this.getViewFromPathString(ctrl.view, pathJoined);
							
							//TODO inheritance of view/model from parent ctrl
						}
						
						//recurse children
						for (var a = 0; a < dom.children.length; a++)
						{
							var childDOM = dom.children[a];
							var childCtrl = this.addCtrl(childDOM, ctrl.model);
							if (childCtrl)
								ctrl.addChild(childCtrl);
							//console.log(childDOM);
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
		var view;
	
		var classNamesJoined = dom.className;
		if (classNamesJoined.length > 0)
		{		
			var classNames = classNamesJoined.split(' ');
			var className = classNames[0];
			var firstChar = className[0];
			
			if (isNaN(firstChar)) //if first character is a letter...
			{
				if (firstChar === firstChar.toUpperCase()) //and it's uppercase...
				{
					//...it's a classname
					className += 'View';
					var Class = window[className]; //TODO make object where this is found, optional.
					if (Class)
					{
						view = new Class(app);
						view.model = model; //by default, set to what was passed in; elaborate (below) if necessary
						
						//set model
						if (dom.hasAttribute('model'))
						{
							var pathJoined = dom.getAttribute('model');
							view.model = this.getModelFromPathString(view.model, pathJoined);
						}
						console.log(view, view.model);

						//any DOM element with a View attached, needs this
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
			view = new Disjunction.Core.NullView(); //null / flyweight
		}
		
		//view.id = id;
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
	
	this.getModelFromPathString = function(model, pathJoined)
	{
		var path = pathJoined.split('.');
		if (path[0] === "") path.shift();
		if (path[path.length-1] === "") path.pop();
		//console.log(path);
		var pathIncremental = "model";
		//walk down the model tree to the appropriate property
		while (path.length > 0)
		{
			var property = path.shift();
			//console.log(model, pathIncremental);
			if (model.hasOwnProperty(property))
			{
				model = model[property];
				pathIncremental += '.' + property;
			}
			else
				throw "Error: "+pathIncremental+" does not have property '"+property+"'.";
		}
		//console.log(model);
		return model;
	}
	
	//TODO access from either an array in the attribute, or a series of <viewref>s in each <ctrl>.
	//TODO possibly remove altogether.
	this.getViewFromPathString = function(view, pathJoined)
	{
		console.log(view);
	
		var path = pathJoined.split('.');
		if (path[0] === "") path.shift();
		var pathIncremental = "view";
		//walk down the view tree to the appropriate property
		
		while (path.length > 0)
		{
			var section = path.shift();
			var className;
			var index;
			
			var elements = section.split('[');
			if (elements.length > 1) //there were indexing brackets
			{
				className = elements[0];
				
				index = parseInt(elements[1].split(']')[0]); //just keep the number
				if (isNaN(index))
					throw "Error: A numeric index must appear between square brackets.";
			}
			else
			{
				className = section;
				index = 0;
			}
			
			var childrenByClassName = view.getChildrenByClassName(className);
			
			if (!childrenByClassName)
				throw "Error: "+pathIncremental+" does not have children with class name '"+className+"'.";
			else
				view = childrenByClassName[index];
			
			pathIncremental += '.' + section;
		}
		
		return view;
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Builder = Disjunction.Core.Builder;