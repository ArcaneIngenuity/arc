MainView = function(app, model)
{
	View.call(this, app, model); //extend base framework class

	this.bounds = new Box2();
	
	//--- abstract framework methods ---//
	this.start = function()
	{
		var dom = this.dom;

		//listeners for DOM events may be set up in View.start()
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		
		//fire up the engines!
		this.onWindowResize();
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
		var devices = disjunction.devices;
		var mouse = devices.array[DEVICE_MOUSE];
		var keyboard = devices.array[DEVICE_KEYBOARD];
		
		//get Pointer
		var pointer = disjunction.pointer;
		
		//***TODO convert raw input to (App or Phase) Model state and View state.
		if (pointer.selectChannel.value)
			console.log( 'hit mouse left mouse button');
			
		if (keyboard.channels[disjunction.constants.KEY_SPACE].value)
			console.log( 'hit spacebar' );
	}
	
	this.output = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
		
		//***TODO render using Phases's Model and View (this) state.
	}
	
	this.outputPost = function(deltaSec)
	{
		var dom = this.dom;
		var model = this.model;
		
		//***TODO render using Phases's Model and View (this) state; always prefer use of output() unless you have special requirements.
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
	
	//--- helper methods ---//
	this.onWindowResize = function()
	{
		//assumes a 2D view (standard)
		var dom = this.dom;
		var width = dom.clientWidth;
		var height = dom.clientHeight;
		this.bounds.setWidth(width);
		this.bounds.setHeight(height);
		
		//***TODO (2D) anything else that needs to change, such as UI-reflow, on resize.
		//***TODO (3D) include any logic which adjusts accordingly, e.g. changing camera aspect ratio.
	}
}

MainView.prototype = Object.create(Disjunction.Core.View.prototype);
MainView.prototype.constructor = MainView;