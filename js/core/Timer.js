Timer = function(apps, periodSec)
{
	this.apps = apps;
	console.log('?', apps)
	for (var i in apps)
	{
		var app = apps[i];
		
		console.log('!', app.id, this);
		app.timer = this;
	}
	
	this.lastUpdateSec = undefined;
	
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
		
		var apps = this.apps;
		var length = apps.length;
		for (var i = 0; i < length; i++)
		{
			var app = apps[i];
			if (app.enabled)
				app.update(deltaSec);
		}
	}
}