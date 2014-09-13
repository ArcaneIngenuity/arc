Timer = function()
{
	for (var id in disjunction)
	{
		var app = disjunction[id];
		app.timer = this;
		disjunction.timer = this;
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
		
		for (var id in disjunction)
		{
			var appArray = disjunction[id];
			for (var i = 0; i < appArray.length; i++)
			{
				//console.log('i', i);
				var app = appArray[i];
				if (app.enabled)
					app.update(deltaSec);
			}
		}
	}
}