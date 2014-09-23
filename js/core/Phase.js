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
				pointer.updateSelectedness();
			}
		}
		//check whether focus has changed
		//this may occur externally via an event-based system like the DOM writing pointer.focus, or be done here, internally, for all views. 
		if (pointer.pollFocus)
			pointer.pollFocus();
		//console.log(focus);
		//update last state for each bound value in this phase's model so we know what has changed for binding purposes
		//TODO same for global / app model
		//TODO in C, we would just provide a list of pairs of 32-bit pointers to the model elements -- new / old -- after model had been fully allocated.
		
		model.updateJournals();
		
		//global update: view.input, ctrl.update, view.output
		var focus = disjunction.pointer.focus;
		var bubble = focus;
		//while (bubble)
		if (bubble)
			bubble = bubble.input(deltaSec); //focus returns next ancestor, and so on... or not. this is the logical opposite of stopPropagation -- instead we propagate if appropriate.
		//....TODO  either this, or we simply set certain views which ALWAYS update input. Or maybe both approaches.
		
		//TODO put dragging in as feature of View
		//if (pointer)
		//	if (pointer.dragging)
		//		pointer.progressDrag();
		ctrl.simulate(deltaSec);
		/*
		//TODO not sure about this part; it really only caches whether a change was made, as a boolean. useful or not useful?
		for (var i = 0; i < this.bindings.length; i++)
		{
			var binding = this.bindings[i];
			binding.markDirtyIfChanged();
		} //now current is fresh
		*/
		//view.updateBindings();
		view.outputRecurse(deltaSec); //render all views from root
	}
};
if (disjunction.WINDOW_CLASSES) 
	window.Phase = Disjunction.Core.Phase;