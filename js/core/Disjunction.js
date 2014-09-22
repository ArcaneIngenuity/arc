Disjunction = function(timer, autoBuildAll, autoStartTimer, skipPrefix)
{
	//classes
	this.core = {};
	this.extn = this.extensions = {};
	
	//instances
	this.apps = {}; //anything not in this map will not be updated by timer.
	this.appBuilder = new AppBuilder(this.apps, skipPrefix);
	
	this.update = function(deltaSec)
	{
		for (var id in this.apps)
		{	
			var app = this.apps[id];
			app.update(deltaSec);
		}	
	}
	
	this.timer = timer;
	this.timer.callback = this.update.bind(this);
	
	if (autoBuildAll)
	{
		this.appBuilder.buildAll();
	}
	
	if (autoStartTimer)
	{
		console.log(this.apps);
		this.timer.start();
	}
}