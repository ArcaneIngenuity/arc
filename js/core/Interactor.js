Interactor = function(app, phase, model, view, ctrl)
{
	this.app = app;
	this.phase = phase;
	this.model = model;
	this.view = view;
	this.ctrl = ctrl;
	
	this.hasInputStarted = function()
	{
		//abstract
	}
	
	this.hasInputFinished = function()
	{
		//abstract
	}
	
	this.inputStart = function()
	{
		//abstract
	}
	
	this.inputFinish = function()
	{
		//abstract
	}
	
	this.inputUpdate = function(deltaSec)
	{
		//abstract
	}
	
	this.ctrlUpdate = function(deltaSec)
	{
		//abstract
	}
}