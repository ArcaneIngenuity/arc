Disjunction.Core.Ctrl = function(app)
{
	this.app = app;
	this.model = undefined;
	
	this.parent = undefined;
	this.children = []; //processing order, if not accessed individually by ids

	/** Tells whether this Task is due to start running; typically a check on model, input devices, Pointer. 
	 * 	@return {Boolean}
	 */ 
	this.isToStart = function()
	{
		//abstract
		//check model and input to see whether to start
	}
	
	/** Tells whether this Task is due to stop running; typically a check on model, input devices, Pointer.
	 * 	@return {Boolean}
	 */
	this.isToStop = function()
	{
		//abstract
		//check model and input to see whether to stop
	}
	
	/** Start this Timeline -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.start = function()
	{
		//abstract
	}
	
	/** Stop this Timeline -- user code to perform one off tasks such as Model member destruction or reset, or View state change. */ 
	this.stop = function()
	{
		//abstract
	}
	
	/** Update Model based on other Model values, on every frame from start()ed till stop()ed (including during the former, but excluding during the latter). */ 
	this.simulate = function(focus)
	{
		//abstract
	}
	
	//*** FINAL METHODS ***//
	
	this.addChild = function(child) //final
	{
		this.children.push(child);
		child.parent = this;
		//this.childrenById[child.id] = child;
	}
	
	//There will never be multiple Ctrls of same type (as child of a single parent), so we need not use ID.
	this.getChildByClassName = function(className) //final
	{
		//return this.childrenByName[id];
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			if (child.className === className)
			{
				return child;
			}
		}
		
		return undefined;
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Ctrl = Disjunction.Core.Ctrl;