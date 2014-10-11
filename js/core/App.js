Disjunction.Core.App = function(id, disjunction) //final
{
	this.id = id; //for compound apps
	this.disjunction = disjunction;
	
	this.services = new Disjunction.Core.ServiceHub(this);
	this.model = undefined;
	this.view = undefined;
	this.ctrl = undefined;
	
	this.focus = undefined; //focus may be reached in many ways -- not just pointer. it's just the active view used for input.
	
	
	this.start = function() //final
	{
		console.log('app start');
		this.ctrl.start();
		this.view.startRecurse();
		this.focus = this.view;
		
		this.focus.dom.focus(); //triggers View.focus() which changes pointer focus (see Builder) -- necessary as in DOM JS we cannot control focus events due to mouse or tabbing.
		//this.view.focus(); //the expected version... outside DOM JS this will work.
	}
	
	this.update = function() //final
	{
		var model = this.model;
		if (model)
			model.updateJournals();
			
		var services = this.services;
		for (var i = 0; i < services.array.length; i++)
		{
			var service = services.array[i];
			service.updateJournals();
		}
		
		var view = this.view;
		var pointer = disjunction.pointer;
		if (pointer) 
		{
			if (view.enabled) //root enabled
			{
				pointer.findTarget(view);
				pointer.updateSelected();
			}
		}
		
		//check whether focus has changed
		//this may occur externally from DOM onfocus event handler, or be done internally based on pointer / input events. 
		//if (this.pollFocus)
		//	this.pollFocus();
		
		//TODO set focus through *user-defined* approaches 
		if (pointer.target && pointer.isSelected)
			pointer.target.focus();
			
		var focus = this.focus;
		
		//root ctrl operates on root model and focused view
		var ctrl = this.ctrl;
		//note we do not pass Pointer in -- we need the focused View for its input, which exists irrespective of whether there is a Pointer available or not
		ctrl.input		(this, model, focus); //TODO should only be the input along with view name - not the whole View. Ctrl should not have access to View.
		ctrl.simulate	(this, model);
		
		view.outputRecurse(this, model); //root view, recurse
	}
	
	
	this.dispose = function() //final
	{
		ctrl.stop(); //if not already stopped
		services.dispose();
		view.dispose();
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.App = Disjunction.Core.App;