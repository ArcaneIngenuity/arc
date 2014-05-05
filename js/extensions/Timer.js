Timer = function(app, periodSec, isUpdatingView, isUpdatingCtrl)
{
	this.app = app;
	
	var timingID = undefined;
	
	this.isUpdatingCtrl = isUpdatingCtrl;
	this.isUpdatingView = isUpdatingView;
	
	//TODO make setter to update this whenever periodMs is changed
	this.periodMs = periodSec * 1000;
	
	this.lastUpdateSec = undefined;
	
	this.start = function()
	{
		timingID = setInterval(update, this.periodMs);
	}
	
	this.stop = function()
	{
		if (timingID)
			clearInterval(timingID);
	}
	
	this.update = function()
	{
		//calc delta time
		var thisUpdateSec = new Date().getTime();
		var lastUpdateSec = this.lastUpdateSec;
		this.lastUpdateSec = thisUpdateSec;
        var deltaSec = (thisUpdateSec - (lastUpdateSec || thisUpdateSec)) / 1000;
		
		var phase = this.app.phaser.phase;
		
		if (this.isUpdatingCtrl)
		{
			phase.ctrl.update(deltaSec);
		}
		
		if (this.isUpdatingView)
		{
			phase.view.update(deltaSec);
		}
	}
	
}