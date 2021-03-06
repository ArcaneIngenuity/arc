MainCtrl = function()
{ 
	Ctrl.call(this); //extend base framework class
	
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
	};
};

MainCtrl.prototype = Object.create(Ctrl.prototype);
MainCtrl.prototype.constructor = MainCtrl;
