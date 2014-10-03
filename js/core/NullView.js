Disjunction.Core.NullView = function()
{
	Disjunction.Core.View.call(this); //extend base framework class
	
	this.bounds = new Box2();
	
	this.start = function() 
	{
		window.addEventListener( 'resize', this.onWindowResize.bind(this), true );
		this.onWindowResize();
	}
	
	this.onWindowResize = function()
	{
		var dom = this.dom;
		var boundingClientRect = dom.getBoundingClientRect();
		this.bounds.x0 = boundingClientRect.left;
		this.bounds.y0 = boundingClientRect.top;
		this.bounds.x1 = boundingClientRect.right;
		this.bounds.y1 = boundingClientRect.bottom;
	}
};

Disjunction.Core.NullView.prototype = Object.create(Disjunction.Core.View.prototype);
Disjunction.Core.NullView.prototype.constructor = Disjunction.Core.NullView;

if (disjunction.WINDOW_CLASSES) 
	window.NullView = Disjunction.Core.NullView;
