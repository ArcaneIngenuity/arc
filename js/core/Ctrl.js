Ctrl = function()
{
	this.app = undefined;
	this.model = undefined;
	this.enabled = true; //used by timing mechanism to know whether to update or not
	
	/** Used to set up resources or values specific to this Ctrl / Model (and thus the entire Phase). */ 
	this.start = function()
	{
		//ABSTRACT: OVERRIDE ME
	}
	
	/** Used to clean up resources or reset values for this Ctrl, if it is no longer needed and can be released. */ 
	this.finish = function()
	{
		//ABSTRACT: OVERRIDE ME
	}
	
	/** Update simulation state by making changes to associated Model. */
	this.simulate = function(deltaSec)
	{
		//ABSTRACT: OVERRIDE ME
		//-input that applies irrespective of View focus, should be run here.
	}
};

