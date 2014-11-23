Foundation.RootView = function(app, model)
{
	View.call(this, app, model); //extend base framework class

	this.bounds = new Box2();
	
	var paragraph;

	//--- abstract framework methods ---//
	this.start = function() 
	{
		console.log('Foundation.RootView.start');
		var dom = this.dom;
		dom.style.display = 'block';
		
		paragraph = dom.getElementsByClassName('input')[0];

		console.log(paragraph);
		
		//listeners for DOM events may be set up in View.start()
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		this.onWindowResize();
		this.show();
	}
	
	this.finish = function()
	{
		var dom = this.dom;
	}
	
	this.output = function()
	{
		var dom = this.dom;
		var model = this.model;
	}
	
	this.outputPost = function()
	{
		var dom = this.dom;
		var model = this.model;
		
		paragraph.innerHTML = model.input;
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
		var dom = this.dom;

		var boundingClientRect = dom.getBoundingClientRect();
		this.bounds.x0 = boundingClientRect.left;
		this.bounds.y0 = boundingClientRect.top;
		this.bounds.x1 = boundingClientRect.right;
		this.bounds.y1 = boundingClientRect.bottom;
		var width = boundingClientRect.width;
		var height = boundingClientRect.height;
	}
}

Foundation.RootView.prototype = Object.create(View.prototype);
Foundation.RootView.prototype.constructor = Foundation.RootView;