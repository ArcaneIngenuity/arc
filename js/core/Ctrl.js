Ctrl = function()
{
	this.app = undefined;
	
	this.model = undefined;
	
	this.enabled = true; //used by timing mechanism to know whether to update or not
	
	
	
	this.start = function()
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s Ctrl.start() is not implemented.");
	}
	
	this.finish = function()
	{
		//console.log(this.app);
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s Ctrl.finish() is not implemented.");
	}
	
	this.update = function(deltaSec)
	{
		/* ABSTRACT: OVERRIDE ME */
		if (app.DEBUG)
			console.warn(this.app.phaser.phase.name+"'s Ctrl.update() is not implemented.");
	}
}

