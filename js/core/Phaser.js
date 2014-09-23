Disjunction.Core.Phaser = function(app) //final
{
	/** Must be defined before it can be set into the manager's map. */
	var array = {};
	this.phase = undefined; //current (public)
	this.app = app;
	
	this.get = function(i)
	{
		return this.array[i];
	}

	this.add = function(phase)
	{
		if (phase.name)
		{
			array[phase.name] = phase;
			phase.setApp(this.app);
			phase.view.disable();
		}
		else
			throw "Cannot register Phase using name '"+phase.name+"'.";
	}
	
	this.change = function(name)
	{
		if (this.phase)
		{
			this.phase.stop();
			this.phase.view.disable();
		}	
		this.phase = array[name];
		this.phase.view.enable();
		this.phase.start();
	}
	
	this.update = function(deltaSec)
	{
		this.phase.update(deltaSec);
	}
	
	this.dispose = function()
	{
		for (var name in this.array)
		{
			array[name].dispose();
		}
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Phaser = Disjunction.Core.Phaser;