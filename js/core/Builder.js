Disjunction.Core.Builder = function(apps)
{
	var DOMPhase = undefined;
	var prefix;
	var commonTagName;
	var appTagName;
	var devicesTagName;
	var deviceTagName;
	var servicesTagName;
	var serviceTagName;
	var phasesTagName;
	var phaseTagName; //sadly necessary due to the fact that custom element need hyphens.
	var pointerTagName;
	var timerTagName;
		
	this.setTagNames = function(usePrefix)
	{
		prefix			= usePrefix ? 'dj-' : '';
			
		commonTagName 	= prefix + 'common';
		appTagName 		= prefix + 'app';
		devicesTagName 	= prefix + 'devices';
		deviceTagName 	= prefix + 'device';
		servicesTagName = prefix + 'services';
		serviceTagName 	= prefix + 'service';
		phasesTagName 	= prefix + 'phases';
		phaseTagName 	= prefix + 'phase' + (prefix === '' ? '-' : ''); //sadly necessary due to the fact that custom element need hyphens.
		pointerTagName 	= prefix + 'pointer';
		timerTagName 	= prefix + 'timer';
		
		DOMPhase = document.registerElement(phaseTagName, {
		  prototype: Object.create(HTMLDivElement.prototype),
		  extends: 'div'
		});
	}
	
	this.tabIndex = undefined;

	var app;
	var currentPhaseName;
	
	var viewIDs = {};
	
	this.buildCommon = function(containerDOM, disjunction)
	{
		var commonDOM = containerDOM.getElementsByTagName(commonTagName)[0];
		var commonServicesDOM = commonDOM.getElementsByTagName(servicesTagName)[0];
	
		//var commonServiceDOMs = commonServicesDOM.getElementsByTagName(serviceTagName);
		
		if (commonServicesDOM) //NOT always true
			this.addServices(commonServicesDOM, disjunction.services);
			
		var commonDevicesDOM = commonDOM.getElementsByTagName(devicesTagName)[0];
		if (commonDevicesDOM) //should always be true
			this.addDevices(commonDevicesDOM, disjunction.devices);
		else
			throw "Error: <devices> must be specified, containing at least one <device>.";
			
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
	}
	
	this.buildApps = function(containerDOM, disjunction)
	{
		var appDOMs = containerDOM.getElementsByTagName(appTagName);

		var length = appDOMs.length;
		for (var i = 0; i < length; i++)
		{
			var appDOM = appDOMs[i];
			if (appDOM.id === "" || appDOM.className === "") //not a zero-length string
			{
				throw "Error: All <app> elements must have id and class attributes.";
			}
			
			var app = this.buildApp(appDOM, disjunction);
			apps[app.id] = app;
		}
	}
	
	//build. usage: <body onload="(new Builder()).buildApp(document.getElementById('Tagger'));">
	this.buildApp = function(appDOM, disjunction)
	{
		var id = appDOM.id;
		var className = appDOM.className;
		app = new Disjunction.Core.App(id, disjunction);
		app.className = className; //TODO put in constructor
		
		var appModelClassName = className+'Model';
		var AppModelClass = window[appModelClassName];
		if (AppModelClass)
		{
			app.model = new AppModelClass();
		}
		/*
		var domDevices = appDOM.getElementsByTagName(devicesTagName)[0];
		if (domDevices) //should always be true
			this.addDevices(domDevices, app.devices);
		else
			throw "Error: <devices> must be specified, containing at least one <device>.";
		
		var domPointer = appDOM.getElementsByTagName(pointerTagName)[0];
		if (domPointer)
		{
			if (domPointer.hasAttribute('device') &&
				domPointer.hasAttribute('x') &&
				domPointer.hasAttribute('y') &&
				domPointer.hasAttribute('select'))
			{
				var device = domPointer.getAttribute('device');
				var x = domPointer.getAttribute('x');
				var y = domPointer.getAttribute('y');
				var select = domPointer.getAttribute('select');
			
				app.setPointer(window[device], window[x], window[y], window[select]);
			}
			else
				throw "Error: <pointer> must have attributes 'device', 'x', 'y', 'select'.";
		}
		else
			throw "Error: <pointer> must be specified, referencing at least one device as found in <devices>.";
		*/
		
		var domServices = appDOM.getElementsByTagName(servicesTagName)[0];
		if (domServices)
			this.addServices(domServices, app.services);

		var domPhases = appDOM.getElementsByTagName(phasesTagName)[0];
		if (domPhases)
			this.addPhases(domPhases);
		else
			throw "Error: <phases> must be specified, containing at least one <phase>.";
			
		//special case devices for DOM JS
		var mouse = disjunction.devices.array[disjunction.constants.DEVICE_MOUSE];
		var keyboard = disjunction.devices.array[disjunction.constants.DEVICE_KEYBOARD];
		//TODO should these be specified on body vs. appDOM? they are shared between all apps.
		appDOM.addEventListener('mousedown', 	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mouseup',		ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mousemove',	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('keydown', 		ES5.bind(keyboard, keyboard.receive));
		appDOM.addEventListener('keyup',		ES5.bind(keyboard, keyboard.receive));
		
		app.phaser.change(currentPhaseName);
		
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
			var element = containerDOM.children[i];
			if (element.tagName.toLowerCase() === deviceTagName)
			{
				var className = element.className;
				var Class = Disjunction.Extensions[className];
				if (Class)
				{
					var deviceConstantName = 'DEVICE_'+className.replace('Device','').toUpperCase();
					var device = devices.add(new Class());
					
					disjunction.constants[deviceConstantName] = device;
					if (disjunction.WINDOW_CONSTANTS)
						window[deviceConstantName] = device; //TODO make the object on which to put this, optional, via "internalConstantsOn"
				}
			}
		}
	}
	
	this.addServices = function(containerDOM, services)//, parentDomRect)
	{
		for (var i = 0; i < containerDOM.children.length; i++)
		{
			var element = containerDOM.children[i];
			if (element.tagName.toLowerCase() === serviceTagName)
			{
				var shortServiceName = element.className;
				var longServiceName = element.className + 'Service';
				var Class = window[longServiceName];
				if (Class)
				{
					var serviceConstantName = 'SERVICE_'+shortServiceName.toUpperCase();
					var service = services.add(new Class());
					
					disjunction.constants[serviceConstantName] = service;
					if (disjunction.WINDOW_CONSTANTS)
						window[serviceConstantName] = service; //TODO make the object on which to put this, optional, via "internalConstantsOn"
				}	
			}
		}
	}
	
	//construct View tree (roughly) by DOM layout - do not include anything that does not have a classname
	this.addChildViews = function(view, element)//, parentDomRect)
	{
		for (var a = 0; a < element.children.length; a++)
		{
			var childElement = element.children[a];
			var id = childElement.id;
			id = id.length > 0 ? id : undefined;
			
			if (id) //if item is not anonymous / has valid id
			{
				//TODO this should be controlled on a Phase-by-Phase or more likely App basis -- anytime we add a View, something should check that the ID / name is unique.
				if (viewIDs.hasOwnProperty(id))
				{
					throw "Multiple Views may not have the same name / ID. '"+id+"' already exists.";
				}
				else
				{
					viewIDs[id] = true; //set id as used
				}
			}
			
			var classNamesJoined = childElement.className;
			
			var childView;
			if (classNamesJoined.length > 0)
			{
				var classNames = classNamesJoined.split(' ');
				var className = classNames[0];//[b];
				
				var Class = window[className+'View'];
				if (Class)
				{
					childView = new Class();
					childView.dom = childElement;
					childView.id = id;
					view.addChild(childView);
					this.prepareElement(childElement, childView);
				}
			}
			
			/*
			var domRect = childElement.getBoundingClientRect();
			
			childView.bounds = new Disjunction.Extensions.Box2();
			childView.bounds.x0 = Math.floor(domRect.left - parentDomRect.left);
			childView.bounds.y0 = Math.floor(domRect.top - parentDomRect.top);
			childView.bounds.setWidth(domRect.width);
			childView.bounds.setHeight(domRect.height);
			*/
			//recurse
			if (childElement.children.length > 0)
				this.addChildViews(childView ? childView : view, childElement);//, domRect); //conditional will skip DOM tree levels that don't have a related View
			else
				this.prepareElement(childElement, view);
			//else	
			//	childView.makeLeaf(); //if no DOM children, set View children array undefined
			
		}
	}
	
	//TODO allow use of <phase> elements containing a single <div> which then becomes the Phase.view.dom.
	//It's either that or <view-div> in <phase> which seems pointless... the idea of <view> element in the DOM(!) is a non sequitur.
	this.addPhases = function(containerDOM)
	{
		//validate - JS only
		var elements = containerDOM.getElementsByTagName('phase');
		if (elements.length)
			throw "Error: In DOM JS, use <"+phaseTagName+"> rather than <phase>.";
	
		for (var a = 0; a < containerDOM.children.length; a++)
		{
			var element = containerDOM.children[a];
			if (element.tagName.toLowerCase() === phaseTagName)
			{
				//assume phase
				this.tabIndex = 0;
				
				var classNamesJoined = element.className;
				if (classNamesJoined.length > 0)
				{
					var classNames = classNamesJoined.split(' ');
				
					//first classname for use as the phase name 
					var className = classNames[0];
					var model, view, ctrl;
				
					var ModelClass = window[className+'Model'];
					if (ModelClass)
						model = new ModelClass();
						
					var ViewClass = window[className+'View'];
					if (ViewClass)
						view = new ViewClass(app, model);
						
					var CtrlClass = window[className+'Ctrl'];
					if (CtrlClass)
						ctrl = new CtrlClass(app, model);

					
					
					this.prepareElement(element, view);

					//console.log(className);
					view.dom = element;
					var domRect = element.getBoundingClientRect();
					view.bounds.x0 = Math.floor(domRect.left);
					view.bounds.y0 = Math.floor(domRect.top);
					view.bounds.setWidth(domRect.width);
					view.bounds.setHeight(domRect.height);
					
					var shortPhaseName = className.replace('Phase','');
					var phase = new Disjunction.Core.Phase(shortPhaseName, model, view, ctrl);
					app.phaser.add(phase);
					if (element.children.length > 0)
						this.addChildViews(view, element, domRect);
					//else	
					//	view.makeLeaf(); //if no DOM children, set View children array undefined

					if (element.current || element.hasAttribute('current')) //WARNING: this could break if there ever emerge a standard property on elements called "current" which is truthy by default
					{
						currentPhaseName = className;
					}
				}
			}
		}
	}
	
	this.prepareElement = function(element, view)
	{
		element.view = view; // bind the view
		element.onfocus = function()
		{
			this.view.focus();
		}
		//element.onblur = function() {this.view.wasFocused = this.view.isFocused; this.view.isFocused = false;};
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Builder = Disjunction.Core.Builder;