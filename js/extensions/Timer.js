Timer = function(app, periodSec)//, isUpdatingView, isUpdatingCtrl)
{
	this.app = app;
	
	var timingID = undefined;

	//TODO make setter to update this whenever periodMs is changed
	this.periodMs = periodSec * 1000;
	
	this.lastUpdateSec = undefined;
	
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
	
	this.update = function()
	{
		//calc delta time
		var thisUpdateSec = new Date().getTime();
		var lastUpdateSec = this.lastUpdateSec;
		this.lastUpdateSec = thisUpdateSec;
        var deltaSec = (thisUpdateSec - (lastUpdateSec || thisUpdateSec)) / 1000;
		
		this.app.update(deltaSec);
	}
	
}