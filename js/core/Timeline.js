/** Timeline groups the lifetime of an operation in one class while respecting MVC and synchronisation boundaries within the framework. */

Timeline = function(app, phase, model, view, ctrl) //abstract
{
	this.app = app;
	this.phase = phase;
	this.model = model;
	this.view = view;
	this.ctrl = ctrl;
	
	/** (call in View.input, Ctrl.update) Lets framework know whether this Task is due to start running; typically a check on model, input devices, Pointer. 
	* @return {Boolean}
	*/ 
	this.isToStart = function()
	{
		//abstract
	}
	
	/** (call in View.input, Ctrl.update) Lets framework know whether this Task is due to finish running; typically a check on model, input devices, Pointer.
	* @return {Boolean}
	*/ 
	this.isToFinish = function()
	{
		//abstract
	}
	
	/** (call in View.input, Ctrl.update) Starts this Task -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.start = function()
	{
		//abstract
	}
	
	/** (call in View.input, Ctrl.update) Finishes this Task -- user code to perform one off tasks such as Model member destruction or reset, or View state change. */ 
	this.finish = function()
	{
		//abstract
	}
	
	/** (call in View.input ONLY) Update Model and/or View based on input, on every frame from start()ed till finish()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.input = function(deltaSec)
	{
		//abstract
	}
	
	//TODO remove, redundant when considering the presence of inputUpdate()?
	/** (call in Ctrl.update ONLY) Update Model based on other Model values (incl. input), on every frame from start()ed till finish()ed (including the former, but excluding the global update on which the latter occurred). */ 
	this.simulate = function(deltaSec)
	{
		//abstract
	}
}