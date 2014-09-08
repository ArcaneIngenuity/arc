ThoughtBubbleView = function(app, model)
{
	View.call(this, app, model); //extend base framework class

	this.bounds = new Box2();
	console.log(this.app);
	var monsterService = this.app.services.get(SERVICE_MONSTER);
	

	//--- abstract framework methods ---//
	this.start = function()
	{
		var dom = this.dom;
		var model = this.model;

		this.show();		
	}
	
	this.finish = function()
	{
		var dom = this.dom;
	}
	
	this.input = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
	
		//get InputHub & Inputs containing InputChannels
		var input = this.app.input;
		var mouse = input.array[INPUT_MOUSE];
		var keyboard = input.array[INPUT_KEYBOARD];
		
		//get Pointer
		var pointer = this.app.pointer;
	}
	
	this.output = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
	}

	this.show = function()
	{
		var dom = this.dom;
		var style = dom.style;
		style.visibility = "visible";
		style.display = "block";
	}
	
	this.hide = function()
	{
		var dom = this.dom;
		var style = dom.style;
		style.visibility = "hidden";
		style.display = "none";
	}
}

MainView.prototype = Object.create(View.prototype);
MainView.prototype.constructor = MainView;