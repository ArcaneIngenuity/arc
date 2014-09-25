MainView = function()
{
	View.call(this); //extend base framework class

	this.bounds = new Box2();

	
	//--- abstract framework methods ---//
	this.start = function() 
	{
		var dom = this.dom;
		dom.style.display = 'block';
		
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
	

	}
	
	
	this.output = function(deltaSec)
	{
	}
	
	this.outputPost = function(deltaSec)
	{
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
		var width = dom.clientWidth;
		var height = dom.clientHeight;
		console.log(width, height);
		this.bounds.setWidth(width);
		this.bounds.setHeight(height);
	}
	
	this.getAspectRatio = function()
	{
		return window.innerWidth / window.innerHeight;
	}
}

MainView.prototype = Object.create(View.prototype);
MainView.prototype.constructor = MainView;