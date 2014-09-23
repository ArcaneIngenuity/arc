MainCtrl = function(app, model)
{
	Ctrl.call(this, app, model); //extend base framework class
	
	/** Used to set up resources or values specific to this Ctrl / Model (and thus the entire Phase). */ 
	this.start = function() //abstract
	{
	};
	
	/** Used to clean up resources or reset values for this Ctrl, if it is no longer needed and can be released. */ 
	this.finish = function() //abstract
	{
	};
	
	/** Update simulation state by making changes to associated Model. */
	this.simulate = function(deltaSec) //abstract
	{
		//NOTE input that applies irrespective of View focus, should be run here.

		var model = this.model;
	};
};

MainCtrl.prototype = Object.create(Disjunction.Core.Ctrl.prototype);
MainCtrl.prototype.constructor = MainCtrl;
