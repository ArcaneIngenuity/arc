Phase = function(name, model, view, ctrl) //(abstract) Updatable, Disposable
{
	/** Must be defined before it can be set into the manager's map. should be unique. */
	this.name  = name;
	
	this.model = model; 
	this.view  = view; //root

	this.ctrl  = ctrl;
	this.view.model = model; //inject
	this.ctrl.model = model; //inject
	
	this.app = undefined;
                       
	this.setApp = function(app)
	{
		this.app = app;                    
		this.ctrl.app = app;
		this.view.app = app;
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
	}
	
	this.finish = function() //final
	{
		this.ctrl.finish();
		this.view.finish();
	}
	
	this.dispose = function() //final
	{
		ctrl.dispose();
		view.dispose();
	}
	
	
	this.update = function(deltaSec) //final
	{
		//use global input to refresh focus if necessary.
		var view = this.view;
		var ctrl = this.ctrl;
		var pointer = this.app.pointer;

		if (pointer) 
		{
			if (view.enabled) //root enabled
			{
				pointer.findTarget(view);
				pointer.updateSelectedness();
			}
		}

		//check whether focus has changed
		//this may occur externally via an event-based system like the DOM writing pointer.focus, or be done here, internally, for all views. 
		if (pointer.pollFocus)
			pointer.pollFocus();
		
		//global update: view.input, ctrl.update, view.output
		var focus = this.app.pointer.focus;
		if (focus) 
			focus.input(deltaSec); //single view gets input processed
		//TODO put dragging in as feature of View
		//if (pointer)
		//	if (pointer.dragging)
		//		pointer.progressDrag();
		ctrl.update(deltaSec);
		//view.updateBindings();
		view.outputRecurse(deltaSec); //render all views from root
	}
	
	/*
	this.changeFocus = function(view)
	{
		console.log('phase.changeFocus');
		this.focus.loseFocus();
		this.focus = view;
		this.focus.gainFocus();
	}
	*/
};