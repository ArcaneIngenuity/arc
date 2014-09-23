Disjunction.Extensions.IntervalTimer = function(periodSec)
{
	Timer.call(this); //super-constructor (params after this)
	
	var timingID = undefined;

	//TODO make setter to update this whenever periodMs is changed
	this.periodMs = periodSec * 1000;
	
	this.start = function()
	{
		timingID = setInterval(update, this.periodMs);
		//In C: running = true; while(running) this.update()
	}
	
	this.stop = function()
	{
		if (timingID)
			clearInterval(timingID);
		//In C: running = false;
	}	
}

Disjunction.Extensions.IntervalTimer.prototype = Object.create(Disjunction.Core.Timer.prototype);
Disjunction.Extensions.IntervalTimer.prototype.constructor = Disjunction.Extensions.IntervalTimer;

if (disjunction.WINDOW_CLASSES) 
	window.IntervalTimer = Disjunction.Extensions.IntervalTimer;