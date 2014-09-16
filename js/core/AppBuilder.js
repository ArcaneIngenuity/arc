//TODO all instances of 'window' should be replaced with 'dj' if  we separate things out
//TODO documentation saying what attributes (e.g. onfocus) not to implement when using this class!
AppBuilder = function(timer)
{
	var appTagName = 'app';
	var devicesTagName = 'devices';
	var deviceTagName = 'device';
	var servicesTagName = 'services';
	var serviceTagName = 'service';
	var phasesTagName = 'phases';
	var phaseTagName = 'phase-div';
	var pointerTagName = 'pointer';

	this.tabIndex = undefined;

	var app;
	var currentPhaseName;
	
	var viewIDs = {};
	
	var DOMPhase = document.registerElement(phaseTagName, {
	  prototype: Object.create(HTMLDivElement.prototype),
	  extends: 'div'
	});
	
	this.buildAll = function()
	{
		if (!window.disjunction) // don't clear the existing object if there is one on window
		{
			disjunction = {};
		}
	
		var appDOMs = document.getElementsByTagName(appTagName);

		var length = appDOMs.length;
		for (var i = 0; i < length; i++)
		{
			var appDOM = appDOMs[i];
			if (appDOM.id === "" || appDOM.className === "") //not a zero-length string
			{
				throw "Error: All <app> elements must have id and class attributes.";
			}
			
			var app = this.buildOne(appDOM);
			this.addApp(app);
		}
		//console.log('disjunction', disjunction);
	}
	
	//build. usage: <body onload="(new AppBuilder()).buildOne(document.getElementById('Tagger'));">
	this.buildOne = function(appDOM)
	{
		//console.log(appDOM);
		var id = appDOM.id;
		var className = appDOM.className;
		app = new App(id);
		app.className = className; //TODO put in constructor
		
		var appModelClassName = className+'Model';
		var AppModelClass = window[appModelClassName];
		if (AppModelClass)
		{
			app.model = new AppModelClass();
		}
		
		var domDevices = appDOM.getElementsByTagName(devicesTagName)[0];
		if (domDevices) //should always be true
			this.addDevices(domDevices);
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
			
				console.log(device, x, y, select);
				app.setPointer(window[device], window[x], window[y], window[select]);
				console.log(app.pointer);
			}
			else
				throw "Error: <pointer> must have attributes 'device', 'x', 'y', 'select'.";
		}
		else
			throw "Error: <pointer> must be specified, referencing at least one device as found in <devices>.";
			
		var domServices = appDOM.getElementsByTagName(servicesTagName)[0];
		if (domServices) //NOT always true
			this.addServices(domServices);

		var domPhases = appDOM.getElementsByTagName(phasesTagName)[0];
		if (domPhases) //should always be true
			this.addPhases(domPhases);
		else
			throw "Error: <phases> must be specified, containing at least one <phase>.";
			

		var mouse = app.input.array[INPUT_MOUSE];
		var keyboard = app.input.array[INPUT_KEYBOARD];
		//TODO should these be specified on body vs. appDOM? they are shared between all apps.
		appDOM.addEventListener('mousedown', 	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mouseup',	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('mousemove',	ES5.bind(mouse, mouse.receive));
		appDOM.addEventListener('keydown', 	ES5.bind(keyboard, keyboard.receive));
		appDOM.addEventListener('keyup',		ES5.bind(keyboard, keyboard.receive));
		
		app.phaser.change(currentPhaseName);
		
		app.timer = timer;
		
		return app;
	}
	
	//(JS-only) Multi-app pages
	this.buildByIds = function(ids)
	{
		if (!window.disjunction) // don't clear the existing object if there is one on window
		{
			disjunction = {};
		}
		
		for (var i in ids)
		{
			var id = ids[i];

			var app = this.buildById(id);
			this.addApp(app);
		}
		//console.log('disjunction', disjunction);
	}
	
	//find and build
	this.buildById = function(id)
	{
		var appDOM = document.getElementById(id);
		return this.buildOne(appDOM);
	}
	
	this.addApp = function(app)
	{
		var className = app.className;
		
		if (!disjunction[className]) //hold refs to multiple instances of same app 
		{
			disjunction[className] = [];
		}
		
		var appArray = disjunction[className];
		appArray.push(app);
	}
	
	this.addDevices = function(domContainer)//, parentDomRect)
	{
		for (var i = 0; i < domContainer.children.length; i++)
		{
			var element = domContainer.children[i];
			if (element.tagName.toLowerCase() === deviceTagName)
			{
				var className = element.className;
				var Class = window[className];
				if (Class)
				{
					//create it, add it to InputManager, and store it's index globally (TODO -- later make index storage location changeable as it may not be wanted on window)
					var deviceName = 'INPUT_'+className.replace('Device','').toUpperCase();
					window[deviceName] = app.input.add(new Class());
				}	
			}
		}
	}
	
	this.addServices = function(domContainer)//, parentDomRect)
	{
		for (var i = 0; i < domContainer.children.length; i++)
		{
			var element = domContainer.children[i];
			if (element.tagName.toLowerCase() === serviceTagName)
			{
				var shortServiceName = element.className;
				var longServiceName = element.className + 'Service';
				var Class = window[longServiceName];
				if (Class)
				{
					//var shortServiceName = className.replace('Service','').toUpperCase();
					//create it, add it to InputManager, and store it's index globally (TODO -- later make index storage location changeable as it may not be wanted on window)
					window['SERVICE_'+shortServiceName.toUpperCase()] = app.services.add(new Class());
				}	
			}
		}
	}
	
	//construct disjunction View tree (roughly) by DOM layout - do not include anything that does not have a classname
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
			
			childView.bounds = new Box2();
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
	this.addPhases = function(domContainer)
	{
		for (var a = 0; a < domContainer.children.length; a++)
		{
			var element = domContainer.children[a];
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
					var phase = new Phase(shortPhaseName, model, view, ctrl);
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
			//console.log('onfocus', element);
			this.view.takeFocus();
		}
		//element.onblur = function() {this.view.wasFocused = this.view.isFocused; this.view.isFocused = false;};
	}
}