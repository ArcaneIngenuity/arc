//TODO listen for connecting / disconnecting devices
disjunction.core.DeviceHub = function(dom)
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
	
	//add device as reader and let function create the associate map in a single exclusive array. (This makes passing the map array alone, to the View hierarchy, simpler.)
	//return the newly assigned index.
	this.add = function(device)
	{
		this.array.push(device);
		
		return this.array.length - 1;
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