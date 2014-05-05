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
	}

	this.start = function() //final
	{
		this.ctrl.start();
		this.view.start();
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
}