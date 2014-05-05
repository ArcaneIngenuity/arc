App = function() //(abstract)
{
	this.DEBUG = true;

	var phaser = this.phaser = new Phaser(this);
	var services = this.services = {};
	var timer = this.timer = undefined; //set from outside
	var model = this.model = undefined; //optional app-wide model
	
	this.dispose = function()
	{
		phaser.dispose();
		
		//serviceManager.dispose();
		for (var name in this.services)
			services[name].dispose();
	}
	
	//TODO global value to enable / disable views, ctrls across all phases?
}