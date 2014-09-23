Disjunction.Core.Ctrl = function(app, model)
{
	this.app = app;
	this.model = model;
	this.phase = undefined; //set by Phase on injection thereinto

	/** Used to set up resources or values specific to this Ctrl / Model (and thus the entire Phase). */ 
	this.start = function()
	{
		//ABSTRACT: OVERRIDE
	};
	
	/** Used to clean up resources or reset values for this Ctrl, if it is no longer needed and can be released. */ 
	this.stop = function()
	{
		//ABSTRACT: OVERRIDE
	};
	
	/** Update simulation state by making changes to associated Model. */
	this.simulate = function(deltaSec)
	{
		//ABSTRACT: OVERRIDE
		//NOTE input that applies irrespective of View focus, should be run here.
	};
};
if (disjunction.WINDOW_CLASSES) 
	window.Ctrl = Disjunction.Core.Ctrl;

