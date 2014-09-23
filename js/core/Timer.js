Disjunction.Core.Timer = function()
{	
	this.lastUpdateSec = undefined;
	this.callback = undefined;
	
	this.start = function()
	{
		//ABSTRACT
	}
	
	this.stop = function()
	{
		//ABSTRACT
	}
	
	this.update = function()
	{
		//calc delta time
		var thisUpdateSec = new Date().getTime();
		var lastUpdateSec = this.lastUpdateSec;
		this.lastUpdateSec = thisUpdateSec;
        var deltaSec = (thisUpdateSec - (lastUpdateSec || thisUpdateSec)) / 1000;
		
		if (this.callback)
		{
			this.callback(deltaSec);
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Timer = Disjunction.Core.Timer;