//"use strict";

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