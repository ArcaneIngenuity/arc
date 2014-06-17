//stores a list of current devices
//listens for connecting / disconnecting devices

//INPUT_KEYBOARD = 0;
//INPUT_MOUSE = 1;

InputDeviceManager = function(domElement)
{
	//var NUM_DEVICES = 2;
	
	//TODO actually in C we should simply realloc when devices are added, and in JS just use array length after pushes.
	//use sufficiently large number for devices
	this.devices = [];
	

	

	this.poll = function()
	{
		var numDevices = this.devices.length;
		for (var i = 0; i < numDevices; i++)
		{
			var device = this.devices[i];
			device.poll();
		}
	}

	this.flush = function()
	{
		var numDevices = this.devices.length;
		for (var i = 0; i < numDevices; i++)
		{
			var device = this.devices[i];
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
	
	//add device as reader and let function create the associate map in a single exclusive array. (This makes passing the map array alone, to the View hierarchy, simpler.)
	//return the newly assigned index.
	this.addDevice = function(device)
	{
		this.devices.push(device);
		
		return this.devices.length - 1;
	}
	
	this.dispose = function()
	{
		//TODO free maps allocated
	}
}