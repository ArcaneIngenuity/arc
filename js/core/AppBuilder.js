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
	var tasksTagName = 'tasks';
	var taskTagName = 'task';

	this.tabIndex = undefined;

	var app;
	var currentPhaseName;
	
	var viewIDs = {};
	//var document = window.document;
	/*
	var DOMApp = document.registerElement(appTagName, {
	  prototype: Object.create(HTMLDivElement.prototype),
	  extends: 'div'
	});
	
	var DOMPhases = document.registerElement(phasesTagName);
	var DOMPhase = document.registerElement(phaseTagName, {
	  prototype: Object.create(HTMLDivElement.prototype),
	  extends: 'div'
	});
	var DOMTasks = document.registerElement(tasksTagName);
	var DOMTask = document.registerElement(taskTagName);
	*/
	
	var DOMPhase = document.registerElement(phaseTagName, {
	  prototype: Object.create(HTMLDivElement.prototype),
	  extends: 'div'
	});
	
	//usage: <body onload="(new AppBuilder()).buildFrom(document.getElementById('Tagger'));">
	this.build = function(domApp)
	{	
		var appId = domApp.id;
		app = window[appId] = new App(appId); //TODO put this on a global dj.apps object?

		var appModelClassName = appId+'Model';
		var AppModelClass = window[appModelClassName];
		if (AppModelClass)
		{
			app.model = new AppModelClass();
		}
		app.view = domApp;
		
		var domDevices = domApp.getElementsByTagName(devicesTagName)[0];
		if (domDevices) //should always be true
		{
			//console.log(domDevices);
			this.addDevices(domDevices);
		}
		var domServices = domApp.getElementsByTagName(servicesTagName)[0];
		if (domServices) //NOT always true
			this.addServices(domServices);

		var domPhases = domApp.getElementsByTagName(phasesTagName)[0];
		if (domPhases) //should always be true
			this.addPhases(domPhases);
			
		var mouse = app.input.array[INPUT_MOUSE];
		var keyboard = app.input.array[INPUT_KEYBOARD];
		domApp.addEventListener('mousedown', 	ES5.bind(mouse, mouse.receive));
		domApp.addEventListener('mouseup',	ES5.bind(mouse, mouse.receive));
		domApp.addEventListener('mousemove',	ES5.bind(mouse, mouse.receive));
		domApp.addEventListener('keydown', 	ES5.bind(keyboard, keyboard.receive));
		domApp.addEventListener('keyup',		ES5.bind(keyboard, keyboard.receive));
		
		app.phaser.change(currentPhaseName);
		
		//TODO from markup
		/*
		var timer = 
		app.timer = //optional - allows ctrls to manipulate frame rate via app.
			new TimerRAF(app, true, true);   //uses requestAnimationFrame
		//	new Timer(app, 0.1, true, true); //uses setInterval (for older browsers)
		app.timer.start();
		*/
		app.timer = timer;
		
		return app;
	}
	
	//(JS-only)
	this.buildByIds = function(ids)
	{
		var apps = [];
		for (var i in ids)
		{
			var id = ids[i];
			var app = this.buildById(id);
			apps.push(app);
		}
		
		return apps;
	}
	
	this.buildById = function(id)
	{
		var domApp = document.getElementById(id);
		return this.build(domApp);
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
					window['INPUT_'+className.replace('Device','').toUpperCase()] = app.input.add(new Class());
					//console.log('INPUT_'+className.toUpperCase(), window['INPUT_'+className.toUpperCase()]);
					//console.log(app.input.array);
				}	
					
			}
				
		}
		
		//TODO put this as attributes on a Pointer object
		app.setPointer(INPUT_MOUSE, MOUSE_X, MOUSE_Y, MOUSE_BUTTON_LEFT);
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
	
	this.addTasks = function(domContainer, phase)//, parentDomRect)
	{
		for (var i = 0; i < domContainer.children.length; i++)
		{
			var element = domContainer.children[i];
			if (element.tagName.toLowerCase() === taskTagName)
			{
				var className = element.className;
				var Class = window[className];
				if (Class)
				{
					var shortTaskName = className.replace('Task','').toUpperCase();
					var task = new Class();
					task.name = shortTaskName;
					//create it, add it to InputManager, and store it's index globally (TODO -- later make index storage location changeable as it may not be wanted on window)
					window['TASK_'+phase.name+'_'+shortTaskName] = phase.tasks.push(task);
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
					//console.log('id', childView.id);
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
					//console.log(className+'View');
					if (ViewClass)
						view = new ViewClass(app, model);
						
					var CtrlClass = window[className+'Ctrl'];
					if (CtrlClass)
						ctrl = new CtrlClass(app, model);

					
					//console.log(model, view, ctrl);
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
					
					var domTasks = element.getElementsByTagName(tasksTagName)[0];
					//console.log('TASKS',  element.getElementsByTagName(tasksTagName)[0]);
					if (domTasks)
						this.addTasks(domTasks, phase);
				}
			}
		}
	}
	
	this.prepareElement = function(element, view)
	{
		element.view = view; // bind the view
		element.onfocus = function()
		{
			this.view.takeFocus();
			//console.dir(this, this.getBoundingClientRect());
		}
		//element.onblur = function() //{this.view.wasFocused = this.view.isFocused; this.view.isFocused = false;};
		//element.onmousedown = "app.phaser.phase.focus = "
		if (element.tabIndex == -1) //only add if not specified in markup
		{
			this.tabIndex++;
			element.tabIndex = this.tabIndex;
		}
	}
}