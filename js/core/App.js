Disjunction.Core.App = function(id, disjunction) //final
{
	this.id = id; //for compound apps
	this.disjunction = disjunction;
	
	this.services = new Disjunction.Core.ServiceHub(this);
	this.model = undefined;
	this.view = undefined;
	this.ctrl = undefined;
	
	
	
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
		if (pointer.target && pointer.isSelected)
			pointer.target.focus();
		
		var ctrl = this.ctrl;
		ctrl.input		(this, model, view);
		ctrl.simulate	(this, model);
		ctrl.output		(this, model, view);
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