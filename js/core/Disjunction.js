Disjunction = function()
{
	//constants
	this.constants =
	{
		X: 0,
		Y: 0,
		Z: 0
	};

	//classes
	var core = this.core = { Disjunction: this };
	var extn = this.extn = this.extensions = {};
	
	//instances
	this.apps = {}; //anything not in this map will not be updated by timer.
	
	this.pointer = undefined;
	this.setPointer = function(deviceIndex, xChannelIndex, yChannelIndex, selectChannelIndex) //final
	{
		var pointer = this.pointer = new core.Pointer();
		pointer.device = this.devices.array[deviceIndex];
		pointer.xChannel = pointer.device.channels[xChannelIndex];
		pointer.yChannel = pointer.device.channels[yChannelIndex];
		pointer.selectChannel = pointer.device.channels[selectChannelIndex];
	}
	
	this.update = function(deltaSec)
	{
		this.devices.poll();
		
		for (var id in this.apps)
		{	
			var app = this.apps[id];
			app.update(deltaSec);
		}
		
		this.devices.flush();
	}

	this.start = function() //public
	{
		this.timer.callback = this.update.bind(this);
		this.timer.start();
	}

	//we can't run all init logic at construction as first the classes to be instantiated must be declared in the disjunction object's class packages.
	this.initialise = function()
	{
		this.builder = new core.Builder(this.apps);
		this.services = new core.ServiceHub(this);
		this.devices = new core.DeviceHub(this);
	}
	
	this.go = function(dom, usePrefixedMarkup, useInternalConstantsOnly, useInternalClassesOnly)
	{
		this.initialise();

		if (!useInternalClassesOnly) //copy classes onto window if desired
		{
			for (var c in this.core) //copy
			{
				window[c] = this.core[c];
			}
			
			for (var e in this.extensions) //copy
			{
				window[e] = this.extensions[e];
			}
		}

		this.builder.useInternalConstantsOnly(useInternalConstantsOnly);
		this.builder.usePrefixedMarkup(usePrefixedMarkup);
		this.builder.buildAll(dom || document, this);
		
		this.start();
	}
};

dj = disjunction = new Disjunction();
delete window[Disjunction]; //remove class now that's its ref'ed from disjunction.core.Disjunction.