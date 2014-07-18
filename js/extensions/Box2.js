Box2 = function(x0, y0, x1, y1)
{
	this.x0 = x0 | 0;
	this.y0 = y0 | 0;
	this.x1 = x1 | 0;
	this.y1 = y1 | 0;
	
	this.intersects = function(other)
	{
		//console.log('/', other instanceof Point2)
		if (other instanceof Point2)
		{
			//inclusive
			//console.log('POINT2');
			return (this.x0 <= other.x && other.x < this.x1 &&
					this.y0 <= other.y && other.y < this.y1);
		}
		else
		if (other instanceof Box2)
		{
			return (this.x0 < other.x1 && this.x1 > other.x0 &&
					this.y0 < other.y1 && this.y1 > other.y0);
		}
	}
	
	this.contains = function(other)
	{
		//console.log('/', other instanceof Point2)
		if (other instanceof Point2)
		{
			return (0 <= other.x && other.x < this.getWidth() &&
					0 <= other.y && other.y < this.getHeight());
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
}