TimerInterval = function(apps, periodSec)
{
	Timer.call(this, apps); //super-constructor (params after this)
	
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

TimerInterval.prototype = Object.create(Timer.prototype);
TimerInterval.prototype.constructor = TimerInterval;