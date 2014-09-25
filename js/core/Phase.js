Disjunction.Core.Phase = function(name, model, view, ctrl) //abstract
{
	/** Must be defined before it can be set into the manager's map. should be unique. */
	this.name  = name;
	
	this.model = model; 
	this.view  = view; //root

	this.ctrl  = ctrl;
	this.view.model = model;
	this.ctrl.model = model;
	
	this.app = undefined;
	
	this.focus = view;
                       
	this.setApp = function(app)
	{
		this.app = app;
		this.ctrl.app = app;
		this.view.app = app;
		this.ctrl.phase = this;
		this.view.phase = this;
		this.view.root = this.view;
		if (this.view.children)
			this.setAppRecurse(this.view);
	}
	
	this.setAppRecurse = function(view)
	{
		var numChildren = view.children.length;
		for (var i = 0; i < numChildren; i++)
		{
			var viewChild = view.children[i];
			viewChild.app = this.app;
			viewChild.phase = this;
			viewChild.root = view.root;
			if (viewChild.children)
				this.setAppRecurse(viewChild);
		}
	}

	this.start = function() //final
	{
		this.ctrl.start();
		this.view.startRecurse();
		
		
		this.focus.dom.focus(); //triggers View.focus() which changes pointer focus (see Builder) -- necessary as in DOM JS we cannot control focus events due to mouse or tabbing.
		//this.view.focus(); //the expected version... outside DOM JS this will work.
	}
	
	this.stop = function() //final
	{
		this.ctrl.stop();
		this.view.stop();
	}
	
	this.dispose = function() //final
	{
		ctrl.dispose();
		view.dispose();
	}
	
	this.update = function(deltaSec) //final
	{
		//use global input to refresh focus if necessary.
		var model = this.model;
		var view = this.view;
		var ctrl = this.ctrl;
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
		
		
		if (model)
			model.updateJournals();
		
		//global update: view.input, ctrl.update, view.output
		this.focus.input(deltaSec);
		ctrl.simulate(deltaSec);
		view.outputRecurse(deltaSec); //render all views from root
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Phase = Disjunction.Core.Phase;