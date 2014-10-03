/** Timeline groups the lifetime of an operation in one class while respecting MVC and synchronisation boundaries within the framework. */

Disjunction.Core.Timeline = function(model) //abstract
{
	Disjunction.Core.Update.call();

	/** Tells whether this Task is due to start running; typically a check on model, input devices, Pointer. 
	 * 	@return {Boolean}
	 */ 
	this.isToStart = function(model, timer, services, devices)
	{
		//abstract
		//check model and input to see whether to start
	}
	
	/** Tells whether this Task is due to stop running; typically a check on model, input devices, Pointer.
	 * 	@return {Boolean}
	 */
	this.isToStop = function(model, timer, services, devices)
	{
		//abstract
		//check model and input to see whether to stop
	}
	
	/** Start this Timeline -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.start = function(model, timer, services)
	{
		//abstract
	}
	
	/** Stop this Timeline -- user code to perform one off tasks such as Model member destruction or reset, or View state change. */ 
	this.stop = function(model, timer, services)
	{
		//abstract
	}
};

Disjunction.Core.App.prototype = new Disjunction.Core.Update();
Disjunction.Core.App.prototype.constructor = Disjunction.Core.App;

if (disjunction.WINDOW_CLASSES) 
	window.Timeline = Disjunction.Core.Timeline;