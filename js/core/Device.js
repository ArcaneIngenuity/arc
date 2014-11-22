Disjunction.Core.Device = function(numChannels) //ABSTRACT / INTERFACE
{
	this.channels = [];
	this.numChannels = numChannels;
	this.eventBased = true; //override; in JS, true by default.
	
	//constructor
	for (var i = 0; i < this.numChannels; i++)
	{
		this.channels[i] = new DeviceChannel();
	}
	
	//passive: use as an input event handler - DOM
	this.receive = function(event)
	{
		//ABSTRACT
	}

	//active: poll underlying system on global update
	this.poll = function()
	{
		//ABSTRACT
	}
	
	this.bindToDOM = function(dom)
	{
		//ABSTRACT
	}
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}
if (disjunction.WINDOW_CLASSES) 
	window.Device = Disjunction.Core.Device;