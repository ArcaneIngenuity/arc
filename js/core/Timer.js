Disjunction.Core.Timer = function()
{	
	this.lastUpdateSec = undefined;
	this.callback = undefined;
	this.deltaSec = undefined;
	
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
        var deltaSec = this.deltaSec = (thisUpdateSec - (lastUpdateSec || thisUpdateSec)) / 1000;
		
		if (this.callback)
		{
			this.callback(deltaSec); //TODO remove param, no longer used
			
			//TODO should simply be: disjunction.update(); -- we never use timers for anything else
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Timer = Disjunction.Core.Timer;