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