Disjunction.Extensions.Box2 = function(x0, y0, x1, y1)
{
	this.x0 = x0;
	this.y0 = y0;
	this.x1 = x1;
	this.y1 = y1;
	
	this.intersects = function(other)
	{
		//console.log('/', other instanceof Point2)
		if (other instanceof Disjunction.Core.Point2)
		{
			//inclusive
			//console.log('POINT2');
			return (this.x0 <= other.x && other.x < this.x1 &&
					this.y0 <= other.y && other.y < this.y1);
		}
		else
		if (other instanceof Disjunction.Extensions.Box2)
		{
			return (this.x0 < other.x1 && this.x1 > other.x0 &&
					this.y0 < other.y1 && this.y1 > other.y0);
		}
	}
	
	this.containsLocal = function(other)
	{
		if (other instanceof Point2)
		{
			return (0 <= other.x && other.x < this.getWidth() &&
					0 <= other.y && other.y < this.getHeight());
		}
		return undefined;
	}
	
	this.containsWorld = function(other)
	{
		if (other instanceof Point2)
		{
			return (this.x0 <= other.x && other.x < this.x1 &&
					this.y0 <= other.y && other.y < this.y1);
		}
		return undefined;
	}
	
	this.getWidth = function()
	{
		return this.x1 - this.x0;
	}
	
	this.getHeight = function()
	{
		return this.y1 - this.y0;
	}
	
	this.setWidth = function(value)
	{
		this.x1 = this.x0 + value;
	}
	
	this.setHeight = function(value)
	{
		this.y1 = this.y0 + value;
	}
	
	this.getCentreX = function()
	{
		return this.x0 + this.getWidth() / 2;
	}
	
	this.getCentreY = function()
	{
		return this.y0 + this.getHeight() / 2;
	}
	
	this.clone = function()
	{
		return new Disjunction.Extensions.Box2(this.x0, this.y0, this.x1, this.y1);
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Box2 = Disjunction.Extensions.Box2;