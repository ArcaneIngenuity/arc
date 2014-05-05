Phaser = function(app) //(final) Updatable, Disposable
{
	/** Must be defined before it can be set into the manager's map. */
	var phases = {};
	this.phase = undefined; //current (public)
	this.app = app;
	
	/*
	this.update = function(deltaSec)
	{
		phase.update(deltaSec);
	}
	*/
	
	this.dispose = function()
	{
		for (var name in this.phases)
		{
			phases[name].dispose();
		}
	}

	this.register = function(phase)
	{
		if (phase.name)
		{
			phases[phase.name] = phase;
			phase.setApp(this.app);
		}
		else
			throw "Cannot register Phase using name '"+phase.name+"'.";
	}
	
	this.change = function(name)
	{
		if (this.phase)
		{
			this.phase.finish();
		}	
		this.phase = phases[name];
		this.phase.start();
	}
}