Disjunction.Core.DeviceChannel = function()
{
	this.value = 0;
	//this.valueLast = undefined;
	this.delta = undefined;
	this.consumed = false; //used to determine if this has already been used during focus-gain
};
if (disjunction.WINDOW_CLASSES) 
	window.DeviceChannel = Disjunction.Core.DeviceChannel;