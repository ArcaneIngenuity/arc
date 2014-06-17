Point2 = function(x, y)
{
	this.x = x | 0;
	this.y = y | 0;
	
	this.clone = function()
	{
		var clone = new Point2(this.x, this.y);
		//console.log('CLONE', clone instanceof Point2)
		return clone;
	}
}

Point2.prototype.constructor = Point2;