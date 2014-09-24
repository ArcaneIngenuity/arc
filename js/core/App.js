Disjunction.Core.App = function(id, disjunction) //final
{
	this.DEBUG = true;

	this.disjunction = disjunction;
	this.id = id; //for compound apps
	
	var phaser = this.phaser = new Disjunction.Core.Phaser(this);
	var services = this.services = new Disjunction.Core.ServiceHub(this);

	var model = this.model = undefined; //optional app-wide model
	
	//TODO remove?
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
		if (model)
			model.updateJournals();
		
		phaser.update(deltaSec);
		
		for (var i = 0; i < services.array.length; i++)
		{
			var service = services.array[i];
			service.updateJournals();
		}
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.App = Disjunction.Core.App;