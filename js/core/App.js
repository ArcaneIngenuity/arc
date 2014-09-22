disjunction.core.App = function(id, disjunction) //final
{
	this.DEBUG = true;

	this.disjunction = disjunction;
	this.id = id; //for compound apps
	
	var phaser = this.phaser = new disjunction.core.Phaser(this);
	var services = this.services = new disjunction.core.ServiceHub(this);

	this.model = undefined; //optional app-wide model
	this.view = undefined; //used when individual disjunction apps are used as modules
	//abstraction of the device used to perform screen pointing - encapsulates hierarchical transformation of device coordinates to focused view's own coordinate system
	
	this.dispose = function() //final
	{
		phaser.dispose();
		input.dispose();
		services.dispose();
	}
	
	this.update = function(deltaSec) //final
	{
		phaser.update(deltaSec);
	}
};