Interactor = function(app, phase, model, view, ctrl)
{
	this.app = app;
	this.phase = phase;
	this.model = model;
	this.view = view;
	this.ctrl = ctrl;
	
	/** Lets framework know whether this Interactor is due to start running; typically just a check on input devices and Pointer. 
	* @return {Boolean}
	*/ 
	this.hasInputStarted = function()
	{
		//abstract
	}
	
	/** Lets framework know whether this Interactor is due to finish running; typically just a check on input devices and Pointer.
	* @return {Boolean}
	*/ 
	this.hasInputFinished = function()
	{
		//abstract
	}
	
	/** Starts using this Interactor -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.inputStart = function()
	{
		//abstract
	}
	
	/** Finishes using this Interactor -- user code to perform one off tasks such as Model member destruction or clearing, or View state change. */ 
	this.inputFinish = function()
	{
		//abstract
	}
	
	/** Update Model based on input, on every frame from inputStart()ed till inputFinish()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.inputUpdate = function(deltaSec)
	{
		//abstract
	}
	
	//TODO remove, redundant when considering the presence of inputUpdate()?
	/** Update Model based on input, on every frame from inputStart()ed till inputFinish()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.ctrlUpdate = function(deltaSec)
	{
		//abstract
	}
}