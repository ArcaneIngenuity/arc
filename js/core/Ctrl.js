Disjunction.Core.Ctrl = function()
{
	Disjunction.Core.Updater.call();
	
	this.app = undefined;
	this.model = undefined;
	
	this.parent = undefined;
	this.children = []; //processing order, if not accessed individually by ids

	/** Tells whether this Task is due to start running; typically a check on model, input devices, Pointer. 
	 * 	@return {Boolean}
	 */ 
	this.isToStart = function(app, model)
	{
		//abstract
		//check model and input to see whether to start
	}
	
	/** Tells whether this Task is due to stop running; typically a check on model, input devices, Pointer.
	 * 	@return {Boolean}
	 */
	this.isToStop = function(app, model)
	{
		//abstract
		//check model and input to see whether to stop
	}
	
	/** Start this Timeline -- user code to perform one off tasks such as Model member construction, or View state change. */ 
	this.start = function(app, model, view)
	{
		//abstract
	}
	
	/** Stop this Timeline -- user code to perform one off tasks such as Model member destruction or reset, or View state change. */ 
	this.stop = function(app, model, view)
	{
		//abstract
	}
	
	this.addChild = function(child) //final
	{
		this.children.push(child);
		child.parent = this;
		//this.childrenById[child.id] = child;
	}
	
	this.getChildById = function(id) //final
	{
		//return this.childrenByName[id];
		var children = this.children;
		var length = children.length;
		for (var i = 0; i < length; i++)
		{
			var child = children[i];
			if (child.id === id)
			{
				return child;
			}
		}
		
		return undefined;
	}
};

Disjunction.Core.Ctrl.prototype = new Disjunction.Core.Updater();
Disjunction.Core.Ctrl.prototype.constructor = Disjunction.Core.Ctrl;

if (disjunction.WINDOW_CLASSES) 
	window.Ctrl = Disjunction.Core.Ctrl;

