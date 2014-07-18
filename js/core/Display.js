Display = function() //(final)
{
	//METHODS
	this.onResize = function()
	{
		var x = this.axes[X].resolution = window.innerWidth;
		var y = this.axes[Y].resolution = window.innerHeight;
		this.axes[X].mid = Math.floor(x / 2);
		this.axes[Y].mid = Math.floor(y / 2);
		this.aspectRatio = x / y;
		this.minorAxis = x == y ? undefined : (x < y ? x : y);
		console.log(x, y);
	}
	
	//CONSTRUCTOR
	this.minorAxis = undefined;
	this.aspectRatio = undefined;
	this.axes = new Array(2);
	this.axes[X] = new DisplayAxis();
	this.axes[Y] = new DisplayAxis();
	window.addEventListener( 'resize', this.onResize.bind(this), true ); //called before View resize handlers, during capturing phase (heading down the tree) - this ensures aspectRatio etc. are already available for Views.
	this.onResize(); //to be, since dj App is usually constructed onReady
}

DisplayAxis = function(resolution, pixelMultiplier)
{
	this.resolution = resolution;
	this.mid = resolution / 2;
	this.pixelMultiplier = pixelMultiplier ? pixelMultiplier : 1; //pixel-stretching
}

X = 0;
Y = 1;
Z = 2;